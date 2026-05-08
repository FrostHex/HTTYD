// ==================== Settings.h ====================
#ifndef SETTINGS_H
#define SETTINGS_H

#include "Control_Camera.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/array.hpp>
#include <functional>

namespace godot
{
    // ======================================================
    // macro-based settings system architecture
    // ======================================================
    //
    // the settings system uses a sophisticated c++ macro approach to:
    //   1. transform compact setting declarations into full c++ code
    //   2. provide type-safe getter/setter methods
    //   3. auto-register with godot's serialization system
    //   4. expose settings to the godot editor ui
    //
    // macro flow:
    //   1. control_main_setting_list -> declares all settings
    //   2. declarative macros expand into actual c++ code
    //   3. macros are specialized for different contexts:
    //        - make_setting_spec for storage specs
    //        - bind_setting for godot integration
    //        - add_if_exposed for ui filtering
    //
    // the "setting list" macro implements x-macros - a technique where
    // one macro argument receives macro fragments that expand after expansion.
    //
    // | parameter       | meaning                        | example             |
    // |----------------|--------------------------------|---------------------|
    // | ui_exposed      | show in settings ui?           | true/false          |
    // | propname        | c++ property name             | language            |
    // | member          | actual member variable        | language            |
    // | type            | native c++ type               | int/bool            |
    // | default value   | initial value                 | language_english    |
    // | key             | save file key                 | "language"          |
    // | variant type    | godot variant enum            | variant::int        |
    // | hint            | property hint for editor      | property_hint_enum  |
    // | hint string     | additional hint data          | "english,chinese"   |
    // | property name   | display name in editor        | "language"          |
    // | label key       | ui translation key            | "entry_language"    |
    // | label fallback  | fallback display string       | "language"          |
    // | label path      | ui node path for this label   | ".../language_label"|
    // | label suffix    | colon or space char           | ":"                 |
    // | setter kind     | custom vs default setter      | setter_custom       |

    // ui_exposed, PropName,    member,         type, default_value,    key,              variant_type,   hint, hint_string,  property_name, label_key, label_fallback, label_path, label_suffix, setter_kind)
    #define SETTINGS_LIST(X) \
        X(true,  Language,      language,       int,  LANGUAGE_ENGLISH, "language",       Variant::INT,   PROPERTY_HINT_ENUM, "English,Chinese", "Language", "entry_language", "Language", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Language_Container/Language_Label", ":", SETTINGS_SETTER_DEFAULT) \
        X(true,  EnableHeadset, enable_headset, bool, false,            "enable_headset", Variant::BOOL,  PROPERTY_HINT_NONE, "", "Enable Headset", "entry_enable_headset", "Enable Headset", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_Label", ":", SETTINGS_SETTER_CUSTOM) \
        X(true,  SubView,       sub_view,       bool, true,             "sub_view",       Variant::BOOL,  PROPERTY_HINT_NONE, "", "Sub View", "entry_sub_view", "Sub View", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/SubView_Container/SubView_Label", ":", SETTINGS_SETTER_DEFAULT) \
        X(true,  Debug,         debug,          bool, true,             "debug",          Variant::BOOL,  PROPERTY_HINT_NONE, "", "Debug", "entry_debug", "Debug Info", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Debug_Container/Debug_Label", ":", SETTINGS_SETTER_DEFAULT) \
        X(false, Badge,         badge,          int,  0,                "badge",          Variant::INT,   PROPERTY_HINT_RANGE, "0,3", "Badge", "", "", "", "", SETTINGS_SETTER_DEFAULT)

    enum Language 
    {
        LANGUAGE_ENGLISH = 0,
        LANGUAGE_CHINESE = 1
    };
    
    enum 
    {
        SETTINGS_SETTER_DEFAULT = 0,
        SETTINGS_SETTER_CUSTOM = 1
    };

    class SaveManager; // forward declaration

    class Settings : public Node
    {
        GDCLASS(Settings, Node);

    public:
        static Settings* singleton;

        Settings();
        ~Settings();

        static Settings* GetSingleton();

        void _ready() override;
        void LoadSettings();
        void SaveSettings();

        // get the list of settings exposed to the UI
        Array GetExposedSettings() const;

        // auto-generated accessors
        #define DECLARE_SETTING_ACCESSORS(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
                void SetVal##PropName(type val); \
                type GetVal##PropName() const { return member; }
                SETTINGS_LIST(DECLARE_SETTING_ACCESSORS)
        #undef DECLARE_SETTING_ACCESSORS

    protected:
        static void _bind_methods();

    private:
        SaveManager* save_manager = nullptr;
        bool is_loading_settings = false;

        // member variables
        #define DECLARE_SETTING_MEMBER(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
                type member = default_value;
                SETTINGS_LIST(DECLARE_SETTING_MEMBER)
        #undef DECLARE_SETTING_MEMBER

        struct SettingSpec 
        {
            const char *key;
            std::function<Variant(const Settings *)> get;
            std::function<void(Settings *, const Variant &)> set;
        };

        const SettingSpec *get_setting_specs(int &count) const;
    };
}

#endif // SETTINGS_H