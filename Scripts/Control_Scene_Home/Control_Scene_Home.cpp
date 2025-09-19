#include "Control_Scene_Home.h"
#include "Control_Main.h" // Add this include to resolve incomplete type

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;


Control_Scene_Home::Control_Scene_Home(Control_Main* main_control, bool enable_headset)
{
    this->main_control = main_control;
    this->enable_headset = enable_headset;
}


Control_Scene_Home::Control_Scene_Home()
{
}

void Control_Scene_Home::_ready()
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }
    viewport_container = get_parent()->get_node<Node>("SubViewportContainer");
    if (enable_headset)
    {
        Node* canvas_layer = viewport_container->get_node<Node>("Viewport/CanvasLayer");
        viewport_container = get_parent()->get_node<Node>("XRToolsViewport2DIn3D");
        canvas_layer->reparent(viewport_container->get_node<Node>("Viewport"));
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
                scene_name = "Scene_Tutorial"; // Practice also goes to Tutorial for now
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
}

Control_Scene_Home::~Control_Scene_Home()
{
}

void Control_Scene_Home::_on_button_pressed(const String& scene_name)
{
    if (main_control) 
    {
        UtilityFunctions::print("Switching to scene: ", scene_name);
        main_control->Switch_Scene(scene_name);
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


void Control_Scene_Home::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_on_button_pressed", "scene_name"), &Control_Scene_Home::_on_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_settings_button_pressed"), &Control_Scene_Home::_on_settings_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_close_button_pressed"), &Control_Scene_Home::_on_close_button_pressed);
}