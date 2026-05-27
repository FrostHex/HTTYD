// ==================== Settings.cpp ====================

#include "Settings.h"
#include "SaveManager.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/dir_access.hpp>

using namespace godot;

Settings* Settings::singleton = nullptr;

Settings::Settings()
{
    if (!singleton)
    {
        singleton = this;
    }
}

Settings::~Settings()
{
    if (singleton == this)
    {
        singleton = nullptr;
    }
}

Settings *Settings::GetSingleton()
{
    return singleton;
}

void Settings::_ready()
{
    save_manager = Object::cast_to<SaveManager>(get_node_or_null(NodePath("SaveManager")));
    ignore_scene_values = false;
    LoadSettings();
}

const Settings::SettingSpec *Settings::get_setting_specs(int &count) const
{
    static const SettingSpec specs[] =
    {
        #define MAKE_SETTING_SPEC(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
        { \
            #member, \
            [](const Settings *self) \
            { \
                return self->GetVal##PropName(); \
            }, \
            [](Settings *self, const Variant &value) \
            { \
                self->SetVal##PropName(static_cast<type>(value)); \
            }, \
            Variant(default_value) \
        },

        SETTINGS_LIST(MAKE_SETTING_SPEC)

        #undef MAKE_SETTING_SPEC
    };

    count = sizeof(specs) / sizeof(specs[0]);

    return specs;
}

void Settings::LoadSettings()
{
    if (!save_manager)
    {
        save_manager = Object::cast_to<SaveManager>(get_node_or_null(NodePath("SaveManager")));
    }

    if (!save_manager)
    {
        return;
    }

    is_loading_settings = true;

    first_run = !save_manager->SettingsFileExists();

    Dictionary settings = save_manager->Settings_Load();

    bool wrote_defaults = false;

    int spec_count = 0;

    const SettingSpec *specs = get_setting_specs(spec_count);

    for (int i = 0; i < spec_count; i++)
    {
        StringName key(specs[i].key);

        Variant value =
            settings.has(key)
            ? settings[key]
            : specs[i].default_value;

        if (!settings.has(key))
        {
            settings[key] = value;
            wrote_defaults = true;
        }

        specs[i].set(this, value);
    }

    is_loading_settings = false;

    notify_property_list_changed();

    if (wrote_defaults)
    {
        save_manager->Settings_Save(settings);
    }
}

void Settings::SaveSettings()
{
    if (!save_manager)
    {
        save_manager = Object::cast_to<SaveManager>(get_node_or_null(NodePath("SaveManager")));
    }

    if (!save_manager)
    {
        return;
    }

    Dictionary settings = save_manager->Settings_Load();

    int spec_count = 0;

    const SettingSpec *specs = get_setting_specs(spec_count);

    for (int i = 0; i < spec_count; i++)
    {
        settings[specs[i].key] = specs[i].get(this);
    }

    save_manager->Settings_Save(settings);
}

void Settings::MarkFirstRunComplete()
{
    first_run = false;
}

void Settings::SetValEnableHeadset(bool val)
{
    enable_headset = val;

    if (val != (bool)(ProjectSettings::get_singleton()->get_setting("xr/openxr/enabled")))
    {
        UtilityFunctions::print("before: ", ProjectSettings::get_singleton()->get_setting("xr/openxr/enabled"), ", after: ", val);
        UtilityFunctions::print("Setting XR enabled to ", val);
        ProjectSettings::get_singleton()->set_setting("xr/openxr/enabled", val);
        ProjectSettings::get_singleton()->set_setting("xr/shaders/enabled", val);
        ProjectSettings::get_singleton()->save();
    }

    if (!is_loading_settings)
    {
        SaveSettings();
    }

    notify_property_list_changed();

    // volumetric clouds are incompatible with VR
    if (val && Settings::GetSingleton()->GetValVolumetricClouds())
    {
        Settings::GetSingleton()->SetValVolumetricClouds(false);
    }
}

void Settings::SetValVolumetricClouds(bool val)
{
    volumetric_clouds = val;

    // volumetric clouds are incompatible with VR
    if (val && Settings::GetSingleton()->GetValEnableHeadset())
    {
        Settings::GetSingleton()->SetValEnableHeadset(false);
    }

    if (!is_loading_settings)
    {
        SaveSettings();
    }

    notify_property_list_changed();
}

// ------------------------------------------------------
// generate generic setter
// ------------------------------------------------------
#define GENERATE_SETTER_DEFAULT(PropName, member, type) \
void Settings::SetVal##PropName(type val) \
{ \
    member = val; \
    \
    if (!is_loading_settings) \
    { \
        SaveSettings(); \
    } \
    \
    notify_property_list_changed(); \
}

// ------------------------------------------------------
// custom setters generate nothing
// ------------------------------------------------------

#define GENERATE_SETTER_CUSTOM(PropName, member, type)

// ------------------------------------------------------
// dispatch helper
// ------------------------------------------------------

#define GENERATE_SETTER_SELECT(kind, PropName, member, type) \
    GENERATE_SETTER_##kind(PropName, member, type)

// ------------------------------------------------------
// expand SETTINGS_LIST
// ------------------------------------------------------

#define GENERATE_SETTING_SETTER(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
    GENERATE_SETTER_SELECT(setter, PropName, member, type)

SETTINGS_LIST(GENERATE_SETTING_SETTER)

// ------------------------------------------------------
// cleanup
// ------------------------------------------------------

#undef GENERATE_SETTING_SETTER
#undef GENERATE_SETTER_SELECT
#undef GENERATE_SETTER_DEFAULT
#undef GENERATE_SETTER_CUSTOM

// ======================================================
// runtime mapping:
// DEFAULT -> false
// CUSTOM  -> true
// ======================================================

#define IS_CUSTOM_DEFAULT false
#define IS_CUSTOM_CUSTOM  true

#define GET_IS_CUSTOM(kind) \
    IS_CUSTOM_##kind

// ======================================================
// godot property bindings
// ======================================================

void Settings::_bind_methods()
{
    ClassDB::bind_method(
        D_METHOD("GetExposedSettings"),
        &Settings::GetExposedSettings);

    ClassDB::bind_method(
        D_METHOD("LoadSettings"),
        &Settings::LoadSettings);

    ClassDB::bind_method(
        D_METHOD("SaveSettings"),
        &Settings::SaveSettings);

    #define BIND_SETTING(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
        ClassDB::bind_method( \
            D_METHOD(#PropName "_setter", "value"), \
            &Settings::SetVal##PropName); \
        \
        ClassDB::bind_method( \
            D_METHOD(#PropName "_getter"), \
            &Settings::GetVal##PropName); \
        \
        ADD_PROPERTY( \
            PropertyInfo( \
                static_cast<Variant::Type>(variant_type), \
                #PropName, \
                hint, \
                hint_string), \
            #PropName "_setter", \
            #PropName "_getter");

    SETTINGS_LIST(BIND_SETTING)

    #undef BIND_SETTING
}

// ======================================================
// get settings display to UI
// ======================================================

Array Settings::GetExposedSettings() const
{
    Array arr;

    #define ADD_IF_EXPOSED(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
        if (display) \
        { \
            Dictionary dict; \
            \
            dict["prop_name"] = String(#PropName); \
            dict["member"] = String(#member); \
            dict["variant_type"] = static_cast<int>(variant_type); \
            dict["hint"] = static_cast<int>(hint); \
            dict["hint_string"] = String(hint_string); \
            dict["is_custom"] = GET_IS_CUSTOM(setter); \
            \
            arr.push_back(dict); \
        }

    SETTINGS_LIST(ADD_IF_EXPOSED)

    #undef ADD_IF_EXPOSED

    return arr;
}