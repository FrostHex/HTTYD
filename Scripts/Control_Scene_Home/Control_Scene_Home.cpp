#include "Control_Scene_Home.h"
#include "Control_Main.h" // Add this include to resolve incomplete type

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/window.hpp> // for Window class
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>

using namespace godot;


Control_Scene_Home::Control_Scene_Home()
{
}

void Control_Scene_Home::_ready()
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
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
    const char* button_names[] = { "Button_TD", "Button_Tutorial", "Button_Practice" };
    for (const char* btn_name : button_names) 
    {
        Node *button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/" + String(btn_name));
        if (button) 
        {
            String scene_name;
            if (String(btn_name) == "Button_Tutorial") {
                scene_name = "Scene_Tutorial";
            } else if (String(btn_name) == "Button_Practice") {
                // scene_name = "Scene_Tutorial"; // Practice also goes to Tutorial for now
            } else {
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
        language_combo->connect("item_selected", Callable(this, "_on_language_changed"));
        // Set initial value
        language_combo->call("select", control_main->GetValLanguage());
    }

    Node *enable_headset_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/EnableHeadset_Container/EnableHeadset_CheckBox");
    if (enable_headset_checkbox) 
    {
        enable_headset_checkbox->connect("toggled", Callable(this, "_on_enable_headset_toggled"));
        // Set initial value for Button in toggle mode
        enable_headset_checkbox->set("button_pressed", control_main->GetValEnableHeadset());
    }

    Node *sub_view_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/SubView_Container/SubView_CheckBox");
    if (sub_view_checkbox) 
    {
        sub_view_checkbox->connect("toggled", Callable(this, "_on_sub_view_toggled"));
        // Set initial value for Button in toggle mode
        sub_view_checkbox->set("button_pressed", control_main->GetValSubView());
    }

    Node *debug_checkbox = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/Settings_Panel/Settings_Content/Debug_Container/Debug_CheckBox");
    if (debug_checkbox) 
    {
        debug_checkbox->connect("toggled", Callable(this, "_on_debug_toggled"));
        // Set initial value for Button in toggle mode
        debug_checkbox->set("button_pressed", control_main->GetValDebug());
    }
}

Control_Scene_Home::~Control_Scene_Home()
{
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
        UtilityFunctions::print("Language changed to: ", index == 0 ? "English" : "中文");
    }
}

void Control_Scene_Home::_on_enable_headset_toggled(bool pressed)
{
    if (control_main) 
    {
        control_main->SetValEnableHeadset(pressed);
        UtilityFunctions::print("Enable Headset toggled: ", pressed);
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


void Control_Scene_Home::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_on_button_pressed", "scene_name"), &Control_Scene_Home::_on_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_settings_button_pressed"), &Control_Scene_Home::_on_settings_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_close_button_pressed"), &Control_Scene_Home::_on_close_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_language_changed", "index"), &Control_Scene_Home::_on_language_changed);
    ClassDB::bind_method(D_METHOD("_on_enable_headset_toggled", "pressed"), &Control_Scene_Home::_on_enable_headset_toggled);
    ClassDB::bind_method(D_METHOD("_on_sub_view_toggled", "pressed"), &Control_Scene_Home::_on_sub_view_toggled);
    ClassDB::bind_method(D_METHOD("_on_debug_toggled", "pressed"), &Control_Scene_Home::_on_debug_toggled);
}