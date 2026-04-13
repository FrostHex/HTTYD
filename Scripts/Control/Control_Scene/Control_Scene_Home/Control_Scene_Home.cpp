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
#include <godot_cpp/classes/time.hpp>

using namespace godot;


Control_Scene_Home::Control_Scene_Home()
{
}

void Control_Scene_Home::_ready()
{
    // Hook into Sky3D TimeOfDay so home scene can track and initialize sky time.
    Node *scene_root = get_parent();
    if (scene_root)
    {
        time_of_day = scene_root->get_node_or_null(NodePath("Sky3D/TimeOfDay"));
        if (time_of_day)
        {
            SyncSkyTime();
            _connect_sky_signals();
        }
        else
        {
            UtilityFunctions::printerr("Control_Scene_Home: Could not find Sky3D/TimeOfDay");
        }
    }


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
    
    // Get settings panel reference
    settings_panel = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel");
    
    // Connect Close button
    Node *close_button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Close_Button");
    if (close_button) 
    {
        close_button->connect("pressed", Callable(this, "_on_close_button_pressed"));
    }

    // Connect settings controls
    Node *language_combo = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Language_Container/Language_ComboBox");
    if (language_combo) 
    {
        // Set initial value BEFORE connecting signal to avoid unintended emission
        language_combo->call("select", control_main->GetValLanguage());
        language_combo->connect("item_selected", Callable(this, "_on_language_changed"));
    }

    Node *enable_headset_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_CheckBox");
    if (enable_headset_checkbox) 
    {
        // Set initial value WITHOUT emitting signal, then connect
        enable_headset_checkbox->call("set_pressed_no_signal", control_main->GetValEnableHeadset());
        enable_headset_checkbox->connect("toggled", Callable(this, "_on_enable_headset_toggled"));
    }

    Node *sub_view_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/SubView_Container/SubView_CheckBox");
    if (sub_view_checkbox) 
    {
        // Set initial value WITHOUT emitting signal, then connect
        sub_view_checkbox->call("set_pressed_no_signal", control_main->GetValSubView());
        sub_view_checkbox->connect("toggled", Callable(this, "_on_sub_view_toggled"));
    }

    Node *debug_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Debug_Container/Debug_CheckBox");
    if (debug_checkbox) 
    {
        // Set initial value WITHOUT emitting signal, then connect
        debug_checkbox->call("set_pressed_no_signal", control_main->GetValDebug());
        debug_checkbox->connect("toggled", Callable(this, "_on_debug_toggled"));
    }

    // Test Dorectly
    // control_main->Switch_Scene("Scene_TD");
}

Control_Scene_Home::~Control_Scene_Home()
{
}

void Control_Scene_Home::_connect_sky_signals()
{
    if (!time_of_day)
    {
        return;
    }

    Callable on_time_changed = Callable(this, "_on_sky_time_changed");
    if (!time_of_day->is_connected("time_changed", on_time_changed))
    {
        time_of_day->connect("time_changed", on_time_changed);
    }
}

void Control_Scene_Home::SyncSkyTime()
{
    if (!time_of_day)
    {
        return;
    }

    Time *time_singleton = Time::get_singleton();
    if (!time_singleton)
    {
        UtilityFunctions::printerr("Control_Scene_Home: Time singleton is unavailable");
        return;
    }

    Dictionary datetime_dict = time_singleton->get_datetime_dict_from_system(false);
    if (time_of_day->has_method("set_from_datetime_dict"))
    {
        time_of_day->call("set_from_datetime_dict", datetime_dict);
    }
    else
    {
        time_of_day->set("year", datetime_dict["year"]);
        time_of_day->set("month", datetime_dict["month"]);
        time_of_day->set("day", datetime_dict["day"]);
        if (time_of_day->has_method("set_time"))
        {
            time_of_day->call("set_time", datetime_dict["hour"], datetime_dict["minute"], datetime_dict["second"]);
        }
    }
}

void Control_Scene_Home::_on_sky_time_changed(double value)
{
    // UtilityFunctions::print("Sky3D time changed: ", value);
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
    
    // Set settings entry texts
    Node *settings_label = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Title"));
    if (settings_label) {
        settings_label->set("text", _get_json_text("entry_settings", "Settings"));
    }
    
    Node *headset_label = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_Label"));
    if (headset_label) {
        headset_label->set("text", _get_json_text("entry_enable_headset", "Enable Headset") + ":");
    }
    
    Node *subview_label = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/SubView_Container/SubView_Label"));
    if (subview_label) {
        subview_label->set("text", _get_json_text("entry_sub_view", "Sub View") + ":");
    }
    
    Node *debug_label = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Debug_Container/Debug_Label"));
    if (debug_label) {
        debug_label->set("text", _get_json_text("entry_debug", "Debug Info") + ":");
    }
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

void Control_Scene_Home::_on_language_changed(int index)
{
    if (control_main) 
    {
        control_main->SetValLanguage(index);
        // UtilityFunctions::print("Language changed to: ", index == 0 ? "English" : "Chinese");
        _update_button_texts(); // Update button texts immediately after language change
    }
}

void Control_Scene_Home::_on_enable_headset_toggled(bool pressed)
{
    if (control_main) 
    {
        control_main->SetValEnableHeadset(pressed);
        UtilityFunctions::print("Enable Headset toggled: ", pressed);

        // Update the label text to inform user to restart after changes using JSON text
        if (viewport_container) 
        {
            Node *label_node = viewport_container->get_node_or_null(NodePath("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_Label"));
            if (label_node) {
                String tip_text = _get_json_text("entry_enable_headset_tip", "Enable Headset (Please restart the software to apply changes):");
                label_node->set("text", tip_text);
            }
        }
    }
}

void Control_Scene_Home::_on_sub_view_toggled(bool pressed)
{
    if (control_main) 
    {
        control_main->SetValSubView(pressed);
        UtilityFunctions::print("Sub View toggled: ", pressed);
    }
}

void Control_Scene_Home::_on_debug_toggled(bool pressed)
{
    if (control_main) 
    {
        control_main->SetValDebug(pressed);
        UtilityFunctions::print("Debug toggled: ", pressed);
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
            // 透明/无徽章 - 设置为null或透明纹理
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
    ClassDB::bind_method(D_METHOD("_on_language_changed", "index"), &Control_Scene_Home::_on_language_changed);
    ClassDB::bind_method(D_METHOD("_on_enable_headset_toggled", "pressed"), &Control_Scene_Home::_on_enable_headset_toggled);
    ClassDB::bind_method(D_METHOD("_on_sub_view_toggled", "pressed"), &Control_Scene_Home::_on_sub_view_toggled);
    ClassDB::bind_method(D_METHOD("_on_debug_toggled", "pressed"), &Control_Scene_Home::_on_debug_toggled);
    ClassDB::bind_method(D_METHOD("_on_sky_time_changed", "value"), &Control_Scene_Home::_on_sky_time_changed);
    ClassDB::bind_method(D_METHOD("_update_badge_display"), &Control_Scene_Home::_update_badge_display);
}