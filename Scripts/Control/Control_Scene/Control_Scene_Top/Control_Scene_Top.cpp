#include "Control_Scene_Top.h"

#include "Control_Camera.h"
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input_event_key.hpp>


using namespace godot;

void Control_Scene_Top::_ready()
{
    // UtilityFunctions::print("Control_Scene_Top ready");
    camera_main = get_tree()->get_root()->get_node<Node3D>("Main/Camera_Main");
}

void Control_Scene_Top::SyncSkyTime(Node *time_of_day)
{
    if (!time_of_day)
    {
        return;
    }

    Time *time_singleton = Time::get_singleton();
    if (!time_singleton)
    {
        UtilityFunctions::printerr("Control_Scene_Top: Time singleton is unavailable");
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

void Control_Scene_Top::_input_top(const Ref<InputEvent> &event)
{
    if (!event.is_valid()) 
    {
        return;
    }
	Ref<InputEventKey> key_event = event;
    if (key_event.is_null()) 
    {
        return;
    }
    if (key_event->get_keycode() == Key::KEY_R)
    {
		ReturnHome();
		return;
	}
}

void Control_Scene_Top::ReturnHome() 
{
	// Get the scene tree and switch back to Scene_Home via Control_Main
	UtilityFunctions::print("returning to Scene_Home");
	SceneTree *tree = get_tree();
	if (!tree) 
	{
		UtilityFunctions::printerr("SceneTree not available");
		return;
	}
	Window *root = tree->get_root();
	if (!root) 
	{
		UtilityFunctions::printerr("Root window not available");
		return;
	}

	set_physics_process(false);
	if (dragon_pilot && dragon_pilot->is_inside_tree())
	{
		dragon_pilot->set_physics_process(false);
	}
	if (ctrl_camera && ctrl_camera->is_inside_tree())
	{
		ctrl_camera->set_physics_process(false);
		ctrl_camera->set_process_input(false);
	}

	// reset camera_main
	if (camera_main && camera_main->is_inside_tree() && root) 
	{
		Node *main_node = root->get_node_or_null(NodePath("Main"));
		if (main_node) 
		{
			camera_main->reparent(main_node);
			camera_main->call_deferred("set_transform", Transform3D(Basis(), Vector3(0.0f, 10.0f, 0.0f)));
			Node* xr_origin = camera_main->get_node_or_null(NodePath("XR/XROrigin"));
			if (xr_origin) 
			{
				xr_origin->call_deferred("set_position", Vector3(0.0f, 0.0f, 0.0f));
				Node* sub_viewport_mesh = xr_origin->get_node_or_null(NodePath("XRCamera/SubViewportMesh"));
				if (sub_viewport_mesh)
				{
					sub_viewport_mesh->queue_free();
				}
			}	
			UtilityFunctions::print("Camera_Main restored to original position");
		} else {
			UtilityFunctions::printerr("Could not find Main node to restore camera");
		}
	}
	else if (camera_main && !camera_main->is_inside_tree())
	{
		UtilityFunctions::printerr("camera_main is not in scene tree");
	}
	else
	{
		UtilityFunctions::printerr("camera_main not found when returning to Scene_Home");
	}

	// prioritize using existing cached reference
	if (!control_main) 
	{
		// attempt to locate it again
		Node *cm = root->get_node_or_null(NodePath("Main/Control_Main"));
		if (!cm) 
		{
			cm = root->find_child("Control_Main", /*recursive*/ true, /*owned*/ false);
		}
		control_main = Object::cast_to<Control_Main>(cm);
	}
	if (control_main) 
	{
		control_main->call("Switch_Scene", "Scene_Home");
	} 
	else 
	{
		UtilityFunctions::printerr("Control_Scene_Top: Control_Main not available to switch scene. Run from Main scene to enable navigation.");
	}

    Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
}

void Control_Scene_Top::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("ReturnHome"), &Control_Scene_Top::ReturnHome);
}