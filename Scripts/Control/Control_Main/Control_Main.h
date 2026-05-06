#ifndef CONTROL_MAIN_H
#define CONTROL_MAIN_H

#include "Control_Camera.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <functional>

namespace godot
{
    enum Language 
    {
        LANGUAGE_ENGLISH = 0,
        LANGUAGE_CHINESE = 1
    };

    // ui_exposed, PropName,    member,         type, default_value,    key,              variant_type,   hint, hint_string,  property_name, label_key, label_fallback, label_path, label_suffix, setter_kind)
    #define CONTROL_MAIN_SETTING_LIST(X) \
        X(true,  Language,      language,       int,  LANGUAGE_ENGLISH, "language",       Variant::INT,   PROPERTY_HINT_ENUM, "English,Chinese", "Language", "entry_language", "Language", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Language_Container/Language_Label", ":", CONTROL_MAIN_SETTER_DEFAULT) \
        X(true,  EnableHeadset, enable_headset, bool, false,            "enable_headset", Variant::BOOL,  PROPERTY_HINT_NONE, "", "Enable Headset", "entry_enable_headset", "Enable Headset", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_Label", ":", CONTROL_MAIN_SETTER_CUSTOM) \
        X(true,  SubView,       sub_view,       bool, true,             "sub_view",       Variant::BOOL,  PROPERTY_HINT_NONE, "", "Sub View", "entry_sub_view", "Sub View", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/SubView_Container/SubView_Label", ":", CONTROL_MAIN_SETTER_DEFAULT) \
        X(true,  Debug,         debug,          bool, true,             "debug",          Variant::BOOL,  PROPERTY_HINT_NONE, "", "Debug", "entry_debug", "Debug Info", "Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Debug_Container/Debug_Label", ":", CONTROL_MAIN_SETTER_DEFAULT) \
        X(false, Badge,         badge,          int,  0,                "badge",          Variant::INT,   PROPERTY_HINT_RANGE, "0,3", "Badge", "", "", "", "", CONTROL_MAIN_SETTER_DEFAULT)

    enum 
    {
        CONTROL_MAIN_SETTER_DEFAULT = 0,
        CONTROL_MAIN_SETTER_CUSTOM = 1
    };

    class SaveManager; // forward declaration

    class Control_Main : public Node
    {
        GDCLASS(Control_Main, Node);

    public:
        Control_Main();
        ~Control_Main();
        void _ready();
        void Switch_Scene(const String &scene_name);
        void LoadSettings();
        void SaveSettings();

        // get the list of settings exposed to the UI
        Array GetExposedSettings() const;

        // auto-generated accessors
        #define DECLARE_SETTING_ACCESSORS(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
                void SetVal##PropName(type val); \
                type GetVal##PropName() const { return member; }
                CONTROL_MAIN_SETTING_LIST(DECLARE_SETTING_ACCESSORS)
        #undef DECLARE_SETTING_ACCESSORS

    protected:
        static void _bind_methods();

    private:
        void AttachCamera(const String &scene_name);
        void AttachSunshineClouds(const String &scene_name, bool attach);

        Control_Camera* ctrl_camera = nullptr;
        Node3D* camera_main = nullptr;
        SaveManager* save_manager = nullptr;
        bool is_loading_settings = false;

        // member variables
        #define DECLARE_SETTING_MEMBER(ui_exposed, PropName, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
                type member = default_value;
                CONTROL_MAIN_SETTING_LIST(DECLARE_SETTING_MEMBER)
        #undef DECLARE_SETTING_MEMBER
    };
}

#endif // CONTROL_MAIN_H