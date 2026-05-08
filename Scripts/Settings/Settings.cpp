// ==================== Settings.cpp ====================
#include "Settings.h"
#include "SaveManager.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

Settings* Settings::singleton = nullptr;

Settings::Settings()
{
    if (!singleton)
        singleton = this;
}

Settings::~Settings()
{
    if (singleton == this)
        singleton = nullptr;
}

Settings* Settings::GetSingleton()
{
    return singleton;
}

void Settings::_ready()
{
    save_manager = get_node<SaveManager>("SaveManager");
    LoadSettings();
}

const Settings::SettingSpec *Settings::get_setting_specs(int &count) const
{
    static const SettingSpec specs[] =
    {
        #define MAKE_SETTING_SPEC(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
                    { \
                        key, \
                        [](const Settings *self) { return self->GetVal##PropName(); }, \
                        [](Settings *self, const Variant &value) { self->SetVal##PropName(static_cast<type>(value)); } \
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
        UtilityFunctions::printerr("SaveManager not initialized in LoadSettings()");
        return;
    }
    
    is_loading_settings = true;
    
    Dictionary settings = save_manager->Settings_Load();
    bool wrote_defaults = false;
    int spec_count = 0;
    const SettingSpec *specs = get_setting_specs(spec_count);
    
    for (int i = 0; i < spec_count; i++)
    {
        StringName key(specs[i].key);
        Variant value = settings.has(key) ? settings[key] : specs[i].get(this);
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
        UtilityFunctions::printerr("SaveManager not initialized in SaveSettings()");
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

// ==================== setters ====================
void Settings::SetValEnableHeadset(bool val)
{
    enable_headset = val;
    bool current = ProjectSettings::get_singleton()->get_setting("xr/openxr/enabled");
    if (current != val) 
    {
        ProjectSettings::get_singleton()->set_setting("xr/openxr/enabled", val);
        ProjectSettings::get_singleton()->save();
    }
    if (!is_loading_settings && save_manager) SaveSettings();
    notify_property_list_changed();
}

#define DEFINE_GENERIC_SETTER(PropName, member, type, ...) \
void Settings::SetVal##PropName(type val) \
{ \
    member = val; \
    if (!is_loading_settings && save_manager) SaveSettings(); \
    notify_property_list_changed(); \
}

DEFINE_GENERIC_SETTER(Language, language, int)
DEFINE_GENERIC_SETTER(SubView, sub_view, bool)
DEFINE_GENERIC_SETTER(Debug, debug, bool)
DEFINE_GENERIC_SETTER(Badge, badge, int)

#undef DEFINE_GENERIC_SETTER

void Settings::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetExposedSettings"), &Settings::GetExposedSettings);
    ClassDB::bind_method(D_METHOD("LoadSettings"), &Settings::LoadSettings);
    ClassDB::bind_method(D_METHOD("SaveSettings"), &Settings::SaveSettings);
    
    #define BIND_SETTING(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
        ClassDB::bind_method(D_METHOD(#PropName "_setter", "value"), &Settings::SetVal##PropName); \
        ClassDB::bind_method(D_METHOD(#PropName "_getter"), &Settings::GetVal##PropName); \
        ADD_PROPERTY(PropertyInfo(static_cast<Variant::Type>(variant_type), property_name, hint, hint_string), \
                    #PropName "_setter", #PropName "_getter");

        SETTINGS_LIST(BIND_SETTING)
    #undef BIND_SETTING
}

// get the list of settings exposed to the UI
Array Settings::GetExposedSettings() const
{
    Array arr;

    #define ADD_IF_EXPOSED(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
        if (ui_exposed) \
        { \
            Dictionary dict; \
            dict["prop_name"] = String(#PropName); \
            dict["variant_type"] = static_cast<int>(variant_type); \
            dict["hint"] = static_cast<int>(hint); \
            dict["hint_string"] = String(hint_string); \
            dict["label_key"] = String(label_key); \
            dict["label_fallback"] = String(label_fallback); \
            dict["label_suffix"] = String(label_suffix); \
            dict["is_custom"] = (setter_kind == SETTINGS_SETTER_CUSTOM); \
            arr.push_back(dict); \
        }

        SETTINGS_LIST(ADD_IF_EXPOSED)
    #undef ADD_IF_EXPOSED

    return arr;
}