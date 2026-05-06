#include "Control_Scene_Home.h"
#include "Control_Main.h" // Add this include to resolve incomplete type

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/window.hpp> // for Window class
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/check_button.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/control.hpp>

using namespace godot;


Control_Scene_Home::Control_Scene_Home()
{
}

void Control_Scene_Home::_ready()
{
    if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

    // Get reference to Control_Main
    SceneTree *tree = get_tree();
    if (tree) 
    {
        Window *root = tree->get_root();
        if (root) 
        {
            control_main = Object::cast_to<Control_Main>(root->get_node_or_null(NodePath("Main/Control_Main")));
            if (!control_main) 
            {
                UtilityFunctions::printerr("Control_Scene_Home: Could not find Control_Main at Main/Control_Main");
                return;
            }
        }
    }

    viewport_container = get_parent()->get_node<Node>("SubViewportContainer");
    if (control_main->GetValEnableHeadset())
    {
        Node* canvas_layer = viewport_container->get_node<Node>("Viewport/CanvasLayer");
        viewport_container = get_parent()->get_node<Node>("XRToolsViewport2DIn3D");
        canvas_layer->reparent(viewport_container->get_node<Node>("Viewport"));

        // If XR failed to initialize, restore UI back to SubViewportContainer instead of using XRToolsViewport2DIn3D
        Ref<XRInterface> primary = XRServer::get_singleton()->get_primary_interface();
        bool xr_active = primary.is_valid() && primary->is_initialized();
        if (!xr_active)
        {
            // Move CanvasLayer back to original 2D SubViewport
            Node *xr_viewport = viewport_container->get_node<Node>("Viewport");
            Node *canvas_in_xr = xr_viewport->get_node<Node>("CanvasLayer");
            Node *base_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node *base_viewport = base_container->get_node<Node>("Viewport");
            canvas_in_xr->reparent(base_viewport);
            viewport_container = base_container;
            UtilityFunctions::print("XR initialization failed, restored UI to SubViewportContainer.");
        }
    }

    // Get settings panel/content references and build settings UI
    settings_panel = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel");
    settings_content = settings_panel ? settings_panel->get_node<Node>("Settings_Content") : nullptr;
    _build_settings_entries();

    // Load and set button texts from JSON file based on current language
    _update_button_texts();

    // Get badge icon reference and update display
    badge_icon = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Button_TD/Badge_Icon");
    if (badge_icon) 
    {
        _update_badge_display();
    }
    else 
    {
        UtilityFunctions::printerr("Control_Scene_Home: Could not find Badge_Icon");
    }

    const char* button_names[] = {"Button_TD", "Button_Tutorial", "Button_Practice", "Button_Dodge"};
    for (const char* btn_name : button_names) 
    {
        Node *button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/" + String(btn_name));
        if (button) 
        {
            String scene_name;
            if (String(btn_name) == "Button_Tutorial") 
            {
                scene_name = "Scene_Tutorial";
            } 
            else if (String(btn_name) == "Button_Practice") 
            {
                scene_name = "Scene_Practice";
            } 
            else 
            {
                scene_name = "Scene_" + String(btn_name).replace("Button_", "");
            }
            button->connect("pressed", Callable(this, "_on_button_pressed").bind(scene_name));
        }
    }
    
    // Connect Settings button
    Node *settings_button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Button_Settings");
    if (settings_button) 
    {
        settings_button->connect("pressed", Callable(this, "_on_settings_button_pressed"));
    }
    
    // Connect Close button
    Node *close_button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Close_Button");
    if (close_button) 
    {
        close_button->connect("pressed", Callable(this, "_on_close_button_pressed"));
    }

    // Test Dorectly
    // control_main->Switch_Scene("Scene_TD");
}

Control_Scene_Home::~Control_Scene_Home()
{
}


void Control_Scene_Home::_on_sky_time_changed(double value)
{
    // UtilityFunctions::print("Sky3D time changed: ", value);
}

void Control_Scene_Home::_clear_settings_entries()
{
    if (!settings_content)
    {
        return;
    }

    Array children = settings_content->get_children();
    for (int i = 0; i < children.size(); i++)
    {
        Node *child = Object::cast_to<Node>(children[i]);
        if (child && child->has_meta("dynamic_setting"))
        {
            child->queue_free();
        }
    }
}

void Control_Scene_Home::_build_settings_entries()
{
    if (!control_main || !settings_content)
    {
        return;
    }

    _clear_settings_entries();

    Array settings = control_main->GetExposedSettings();
    for (int i = 0; i < settings.size(); i++)
    {
        Dictionary entry = settings[i];
        String prop_name = String(entry.get("prop_name", ""));
        if (prop_name.is_empty())
        {
            continue;
        }

        int variant_type = static_cast<int>(entry.get("variant_type", Variant::NIL));
        int hint = static_cast<int>(entry.get("hint", PROPERTY_HINT_NONE));
        String hint_string = String(entry.get("hint_string", ""));
        String label_key = String(entry.get("label_key", ""));
        String label_fallback = String(entry.get("label_fallback", prop_name));
        String label_suffix = String(entry.get("label_suffix", ""));
        bool is_custom = static_cast<bool>(entry.get("is_custom", false));

        if (label_fallback.is_empty())
        {
            label_fallback = prop_name;
        }

        HBoxContainer *row = memnew(HBoxContainer);
        row->set_name(prop_name + "_Container");
        row->set_meta("dynamic_setting", true);
        settings_content->add_child(row);

        Label *label = memnew(Label);
        label->set_name(prop_name + "_Label");
        label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        label->set_text(_get_json_text(label_key, label_fallback) + label_suffix);
        label->set_meta("label_key", label_key);
        label->set_meta("label_fallback", label_fallback);
        label->set_meta("label_suffix", label_suffix);
        row->add_child(label);

        if (hint == PROPERTY_HINT_ENUM)
        {
            OptionButton *option = memnew(OptionButton);
            option->set_name(prop_name + "_ComboBox");
            option->set_h_size_flags(Control::SIZE_EXPAND_FILL);

            PackedStringArray items = hint_string.split(",", false);
            for (int idx = 0; idx < items.size(); idx++)
            {
                String item_text = items[idx].strip_edges();
                option->add_item(item_text, idx);
            }

            Variant current = control_main->call(prop_name + "_getter");
            int current_value = static_cast<int>(current);
            option->select(current_value);
            option->connect("item_selected", Callable(this, "_on_setting_enum_changed").bind(prop_name));
            row->add_child(option);
        }
        else if (variant_type == Variant::BOOL)
        {
            CheckButton *check = memnew(CheckButton);
            check->set_name(prop_name + "_CheckBox");
            check->set_focus_mode(Control::FOCUS_NONE);

            Variant current = control_main->call(prop_name + "_getter");
            bool current_value = static_cast<bool>(current);
            check->set_pressed_no_signal(current_value);
            check->connect("toggled", Callable(this, "_on_setting_bool_toggled").bind(prop_name, is_custom));
            row->add_child(check);
        }
    }
}

void Control_Scene_Home::_update_setting_labels()
{
    if (!settings_content)
    {
        return;
    }

    Array children = settings_content->get_children();
    for (int i = 0; i < children.size(); i++)
    {
        Node *row = Object::cast_to<Node>(children[i]);
        if (!row || !row->has_meta("dynamic_setting"))
        {
            continue;
        }

        Array row_children = row->get_children();
        for (int j = 0; j < row_children.size(); j++)
        {
            Label *label = Object::cast_to<Label>(row_children[j]);
            if (!label || !label->has_meta("label_key"))
            {
                continue;
            }

            String label_key = String(label->get_meta("label_key", ""));
            String label_fallback = String(label->get_meta("label_fallback", ""));
            String label_suffix = String(label->get_meta("label_suffix", ""));
            label->set_text(_get_json_text(label_key, label_fallback) + label_suffix);
        }
    }
}

void Control_Scene_Home::_update_button_texts()
{
    if (!control_main || !viewport_container) return;

    // Set button texts
    Node *settings_btn = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Button_Settings"));
    if (settings_btn) {
        settings_btn->set("text", _get_json_text("button_settings", "Settings"));
    }
    
    Node *tutorial_btn = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Button_Tutorial"));
    if (tutorial_btn) {
        tutorial_btn->set("text", _get_json_text("button_tutorial", "Note Book"));
    }
    
    Node *practice_btn = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Button_Practice"));
    if (practice_btn) {
        practice_btn->set("text", _get_json_text("button_practice", "Flight Practice"));
    }
    
    Node *td_btn = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Button_TD"));
    if (td_btn) {
        td_btn->set("text", _get_json_text("button_td", "Test Drive"));
    }
    
    Node *close_btn = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Close_Button"));
    if (close_btn) {
        close_btn->set("text", _get_json_text("button_back", "Back"));
    }
    
    // Set settings title
    Node *settings_label = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Title_Label"));
    if (settings_label) {
        settings_label->set("text", _get_json_text("entry_settings", "Settings"));
    }

    _update_setting_labels();
}

String Control_Scene_Home::_get_json_text(const String& key, const String& fallback)
{
    if (!control_main) return fallback;

    // Determine JSON file based on current language
    String json_file = "res://Media/Text/English.json";
    if (control_main->GetValLanguage() == 1) 
    {
        json_file = "res://Media/Text/Chinese.json";
    }

    // Read JSON file and get specific text
    Ref<FileAccess> f = FileAccess::open(json_file, FileAccess::READ);
    if (f.is_valid()) 
    {
        String content = f->get_as_text();
        f->close();
        
        Ref<JSON> json = memnew(JSON);
        Error parse_result = json->parse(content);
        if (parse_result == OK) 
        {
            Dictionary data = json->get_data();
            if (data.has(key)) {
                return String(data[key]);
            }
        }
    }
    
    return fallback;
}

void Control_Scene_Home::_on_button_pressed(const String& scene_name)
{
    if (control_main) 
    {
        UtilityFunctions::print("Switching to scene: ", scene_name);
        control_main->Switch_Scene(scene_name);
    }
}

void Control_Scene_Home::_on_settings_button_pressed()
{
    if (settings_panel) 
    {
        settings_panel->set("visible", true);
    }
}

void Control_Scene_Home::_on_close_button_pressed()
{
    if (settings_panel) 
    {
        settings_panel->set("visible", false);
    }
}

void Control_Scene_Home::_on_setting_enum_changed(int index, const String& prop_name)
{
    if (!control_main) 
    {
        return;
    }

    control_main->call(prop_name + godot::String("_setter"), index);
    if (prop_name == "Language")
    {
        _update_button_texts();
    }
}

void Control_Scene_Home::_on_setting_bool_toggled(bool pressed, const String& prop_name, bool is_custom)
{
    if (!control_main)
    {
        return;
    }

    control_main->call(prop_name + godot::String("_setter"), pressed);
    UtilityFunctions::print(prop_name, " toggled: ", pressed);

    if (is_custom && prop_name == "EnableHeadset" && settings_content)
    {
        Node *container = settings_content->get_node_or_null(NodePath(prop_name + godot::String("_Container")));
        if (container)
        {
            Label *label_node = Object::cast_to<Label>(container->get_node_or_null(NodePath(prop_name + godot::String("_Label"))));
            if (label_node)
            {
                String tip_text = _get_json_text("entry_enable_headset_tip", "Enable Headset (Please restart the software to apply changes)");
                label_node->set_text(tip_text);
            }
        }
    }
}

void Control_Scene_Home::_update_badge_display()
{
    if (!badge_icon || !control_main) 
    {
        return;
    }

    TextureRect* texture_rect = Object::cast_to<TextureRect>(badge_icon);
    if (!texture_rect) 
    {
        UtilityFunctions::printerr("Badge_Icon is not a TextureRect");
        return;
    }

    int badge_value = control_main->GetValBadge();
    String texture_path;

    switch (badge_value) 
    {
        case 0:
            // transparent/no badge - set to null or a transparent texture.
            texture_rect->set_texture(Ref<Texture2D>());
            texture_rect->set_visible(false);
            break;
        case 1:
            texture_path = "res://Media/Image/badge_1.png";
            break;
        case 2:
            texture_path = "res://Media/Image/badge_2.png";
            break;
        case 3:
            texture_path = "res://Media/Image/badge_3.png";
            break;
        default:
            UtilityFunctions::printerr("Invalid badge value: ", badge_value);
            return;
    }

    if (badge_value > 0) 
    {
        Ref<Texture2D> texture = ResourceLoader::get_singleton()->load(texture_path);
        if (texture.is_valid()) 
        {
            texture_rect->set_texture(texture);
            texture_rect->set_visible(true);
            // UtilityFunctions::print("Badge updated to: ", badge_value, " using texture: ", texture_path);
        } 
        else 
        {
            UtilityFunctions::printerr("Failed to load badge texture: ", texture_path);
            texture_rect->set_visible(false);
        }
    }
}

void Control_Scene_Home::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_on_button_pressed", "scene_name"), &Control_Scene_Home::_on_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_settings_button_pressed"), &Control_Scene_Home::_on_settings_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_close_button_pressed"), &Control_Scene_Home::_on_close_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_setting_enum_changed", "index", "prop_name"), &Control_Scene_Home::_on_setting_enum_changed);
    ClassDB::bind_method(D_METHOD("_on_setting_bool_toggled", "pressed", "prop_name", "is_custom"), &Control_Scene_Home::_on_setting_bool_toggled);
    ClassDB::bind_method(D_METHOD("_on_sky_time_changed", "value"), &Control_Scene_Home::_on_sky_time_changed);
    ClassDB::bind_method(D_METHOD("_update_badge_display"), &Control_Scene_Home::_update_badge_display);
}