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
    // the settings system uses x-macros to:
    //   1. declare all settings in one place
    //   2. auto-generate getters/setters
    //   3. auto-generate serialization specs
    //   4. auto-bind properties to godot
    //   5. auto-generate default setters
    //
    // setter:
    //   DEFAULT -> auto-generate generic setter
    //   CUSTOM  -> setter manually implemented
    //
    // the same token is also mapped into:
    //   DEFAULT -> false
    //   CUSTOM  -> true
    //
    // allowing runtime "is_custom" queries without
    // duplicating information in SETTINGS_LIST
    //
    // | parameter       | meaning                        | example                 |
    // |-----------------|--------------------------------|-------------------------|
    // | PropName        | c++ property name              | Language                |
    // | member          | actual member variable         | language                |
    // | display         | show in settings ui?           | true/false              |
    // | type            | native c++ type                | int/bool                |
    // | default_value   | initial value                  | LANGUAGE_ENGLISH        |
    // | setter          | setter type                    | DEFAULT/CUSTOM          |
    // | variant_type    | godot variant enum             | Variant::INT            |
    // | hint            | property hint for editor       | PROPERTY_HINT_ENUM      |
    // | hint_string     | additional hint data           | "English,Chinese"       |

    //    PropName,           member,             display, type, default_value,    setter,  variant_type,  hint,               hint_string
    #define SETTINGS_LIST(X) \
        X(Language,           language,             true,  int,  LANGUAGE_ENGLISH, DEFAULT, Variant::INT,  PROPERTY_HINT_ENUM,  "English,Chinese") \
        X(EnableHeadset,      enable_headset,       true,  bool, false,            CUSTOM,  Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(SubView,            sub_view,             true,  bool, true,             DEFAULT, Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(VolumetricClouds,   volumetric_clouds,    true,  bool, false,            CUSTOM,  Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(AutoRoll,           auto_roll,            true,  bool, false,            DEFAULT, Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(AutoSaveState,      auto_save_state,      true,  bool, true,             DEFAULT, Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(DebugInfo,          debug_info,           true,  bool, true,             DEFAULT, Variant::BOOL, PROPERTY_HINT_NONE,  "") \
        X(Badge,              badge,                false, int,  0,                DEFAULT, Variant::INT,  PROPERTY_HINT_RANGE, "0,3")

    enum Language
    {
        LANGUAGE_ENGLISH = 0,
        LANGUAGE_CHINESE = 1
    };

    class SaveManager;

    class Settings : public Node
    {
        GDCLASS(Settings, Node);

    public:
        static Settings *singleton;

        Settings();
        ~Settings();

        static Settings *GetSingleton();

        void _ready() override;

        void LoadSettings();
        void SaveSettings();

        // get settings display to the UI
        Array GetExposedSettings() const;

        // ======================================================
        // auto-generated accessors
        // ======================================================

        #define DECLARE_SETTING_ACCESSORS(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
            void SetVal##PropName(type val); \
            type GetVal##PropName() const { return member; }

        SETTINGS_LIST(DECLARE_SETTING_ACCESSORS)

        #undef DECLARE_SETTING_ACCESSORS

    protected:
        static void _bind_methods();

    private:
        SaveManager *save_manager = nullptr;

        bool is_loading_settings = false;

        bool ignore_scene_values = true;

        // ======================================================
        // member variables
        // ======================================================

        #define DECLARE_SETTING_MEMBER(PropName, member, display, type, default_value, setter, variant_type, hint, hint_string) \
            type member = default_value;

        SETTINGS_LIST(DECLARE_SETTING_MEMBER)

        #undef DECLARE_SETTING_MEMBER

        // ======================================================
        // serialization specification
        // ======================================================

        struct SettingSpec
        {
            const char *key;

            std::function<Variant(const Settings *)> get;

            std::function<void(Settings *, const Variant &)> set;

            Variant default_value;
        };

        const SettingSpec *get_setting_specs(int &count) const;
    };
}

#endif // SETTINGS_H