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
    const char* button_names[] = { "Button_TD", "Button_Tutorial" };
    for (const char* btn_name : button_names) 
    {
        Node *button = viewport_container->get_node<Node>("Viewport/CanvasLayer/Control/" + String(btn_name));
        if (button) 
        {
            String scene_name = "Scene_" + String(btn_name).replace("Button_", "");
            button->connect("pressed", Callable(this, "_on_button_pressed").bind(scene_name));
        }
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


void Control_Scene_Home::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_on_button_pressed", "scene_name"), &Control_Scene_Home::_on_button_pressed);
}