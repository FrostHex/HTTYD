#include "Control_Scene_Dodge.h"
#include "Control_Main.h"
#include "Settings.h"

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>

using namespace godot;

Control_Scene_Dodge::Control_Scene_Dodge() {}
Control_Scene_Dodge::~Control_Scene_Dodge() {}

void Control_Scene_Dodge::_bind_methods() 
{
}

void Control_Scene_Dodge::_ready() 
{
	// Hook into Sky3D TimeOfDay so home scene can track and initialize sky time.
    Node *scene_root = get_parent();
    if (scene_root)
    {
        Node *time_of_day = scene_root->get_node_or_null(NodePath("Sky3D/TimeOfDay"));
        if (time_of_day)
        {
			Callable on_time_changed = Callable(this, "_on_sky_time_changed");
			if (!time_of_day->is_connected("time_changed", on_time_changed))
			{
				time_of_day->connect("time_changed", on_time_changed);
			}
			SyncSkyTime(time_of_day);
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

    Control_Scene_Top::_ready();
    ctrl_camera = get_tree()->get_root()->get_node<Control_Camera>("Main/Control_Main/Control_Camera");
	ctrl_camera->call_deferred("Initialize");
}

void Control_Scene_Dodge::_process(double delta)
{
	_process_top(delta);
}

void Control_Scene_Dodge::_input(const Ref<InputEvent> &event) 
{
    _input_top(event);
}