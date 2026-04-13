#include "Control_Scene_Practice.h"
#include "Control_Main.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/core/class_db.hpp>
// font-related headers
#include <godot_cpp/classes/system_font.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/xr_controller3d.hpp>

using namespace godot;

Control_Scene_Practice::Control_Scene_Practice() {}
Control_Scene_Practice::~Control_Scene_Practice() {}

void Control_Scene_Practice::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("_on_back_button_pressed"), &Control_Scene_Practice::_on_back_button_pressed);
}

void Control_Scene_Practice::_ready() 
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
                UtilityFunctions::printerr("Could not find Control_Main at Main/Control_Main");
                return;
            }
        }
    }

	// get the 3D text label reference (Practice paper).
	Node *parent_node = get_parent();
	if (parent_node) {
		practice_label = Object::cast_to<Label3D>(parent_node->get_node_or_null(NodePath("Dragon/PracticePaper/PracticeText")));
		if (!practice_label) {
			UtilityFunctions::printerr("Control_Scene_Practice: Could not find PracticeText label");
		}
	}

	String json_file = "res://Media/Text/English.json";
	if (control_main) 
	{
		int lang = control_main->GetValLanguage();
		if (lang == 1) 
		{
			json_file = "res://Media/Text/Chinese.json";
		}
	}

	// if the current language is Chinese, set Practice text font to a system sans-serif Chinese fallback stack.
	if (control_main && practice_label) {
		if (control_main->GetValLanguage() == 1) 
		{
			Ref<SystemFont> zh_font;
			zh_font.instantiate();
			PackedStringArray font_names;
			// common Chinese font priority on Windows.
			font_names.push_back("SimHei");
			font_names.push_back("Microsoft YaHei");
			zh_font->set_font_names(font_names);
			// apply to the 3D label.
			practice_label->set_font(zh_font);
		}
	}

	// load the practice array using JSON.
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
			if (data.has("practice")) 
			{
				Array practice_array = data["practice"];
				for (int i = 0; i < practice_array.size(); i++) 
				{
					lines.push_back(String(practice_array[i]));
				}
			}
		}
		else 
		{
			UtilityFunctions::printerr("Failed to parse JSON file: " + json_file);
		}
	}
	else 
	{
		UtilityFunctions::printerr("Failed to open JSON file: " + json_file);
	}

	// Set the connection with dragon
	Node *dragon_node = get_parent()->get_node<Node>("Dragon");
    dragon_animator = get_parent()->get_node<Node>("Dragon")->get_node<Dragon_Animator>("Dragon_Animator");
	camera_main = get_tree()->get_root()->get_node<Node3D>("Main/Camera_Main");
	if (!camera_main)
	{
		UtilityFunctions::printerr("Control_Scene_Practice: Could not find Dragon camera_main");
	}
	practice_paper = get_parent()->get_node<Node3D>("Dragon/PracticePaper");
	ctrl_camera = tree->get_root()->get_node<Control_Camera>("Main/Control_Main/Control_Camera");
	ctrl_camera->call_deferred("Initialize");
    if (control_main->GetValEnableHeadset()) 
    {
        dragon_control = memnew(Dragon_Pilot_Joystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control)); // add the dragon control to the dragon node
		dragon_control->set_name("Dragon_Pilot_Joystick"); // set the name of the dragon control node
	}
    else
    {
        dragon_control = memnew(Dragon_Pilot_Keyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control));
		dragon_control->set_name("Dragon_Pilot_Keyboard"); // set the name of the dragon control node
    }
    ctrl_camera->SetDragon_Pilot_(dragon_control); // set the dragon control to the camera control
	dragon_node->call_deferred("set_rotation", Vector3(0.0f, 0.0f, 0.05f));

	// cache the right controller (used for B-button state toggle), following Tutorial.cpp behavior.
	hand_right = nullptr;
	Node *xr_origin = nullptr;
	if (camera_main)
	{
		Node *xr_node = camera_main->get_node_or_null(NodePath("XR"));
		if (xr_node)
		{
			xr_origin = xr_node->get_node_or_null(NodePath("XROrigin"));
		}
	}
	if (xr_origin) {
		hand_right = Object::cast_to<XRController3D>(xr_origin->get_node_or_null(NodePath("RightHand")));
	}

	// initialize dragon state to default.
	is_crisis_state = false;
	if (dragon_control) {
		dragon_control->SetState(DragonState::STATE_DEFAULT);
	}

	// show Practice text (display only the first sentence).
	if (practice_label && lines.size() > 0) {
		String practice_text = lines[0];
		practice_label->set_text(practice_text);
	}
}


void Control_Scene_Practice::_input(const Ref<InputEvent> &event) 
{
	if (!event.is_valid()) return;
	Ref<InputEventKey> key_event = event;
	if (key_event.is_null()) return;
	if (!key_event->is_pressed() || key_event->is_echo()) return; // 只处理按下的首次事件

	// Enter or keypad Enter: toggle dragon state.
	if (key_event->get_keycode() == Key::KEY_ENTER || key_event->get_keycode() == Key::KEY_KP_ENTER) {
		_toggle_dragon_state();
		return;
	}

	// Backspace: return to the home scene.
	if (key_event->get_keycode() == Key::KEY_BACKSPACE) {
		_on_back_button_pressed();
		return;
	}
}

void Control_Scene_Practice::_physics_process(double delta)
{
	// VR controller A/B button logic, following Tutorial.cpp.
	// enable only when the right controller exists and dragon control is in VR mode.
	if (hand_right && control_main && control_main->GetValEnableHeadset()) 
	{
		// A/B are right-hand buttons; Dragon_Pilot_Joystick uses "ax_button" / "by_button".
		float a_val = hand_right->get_float("ax_button");
		float b_val = hand_right->get_float("by_button");
		bool a_now = a_val > 0.5f;
		bool b_now = b_val > 0.5f;

		// edge-triggered: A returns home, B toggles state.
		if (a_now && !a_button_prev) 
		{
			_on_back_button_pressed();
		}
		if (b_now && !b_button_prev) 
		{
			_toggle_dragon_state();
		}
		a_button_prev = a_now;
		b_button_prev = b_now;
	}
}

void Control_Scene_Practice::_toggle_dragon_state()
{
	if (!dragon_control) return;

	// toggle state.
	is_crisis_state = !is_crisis_state;
	
	if (is_crisis_state) 
	{
		// set to crisis state.
		dragon_control->SetState(DragonState::STATE_CRISIS);
		UtilityFunctions::print("Dragon state switched to CRISIS");
	} 
	else 
	{
		// set to default state.
		dragon_control->SetState(DragonState::STATE_DEFAULT);
		UtilityFunctions::print("Dragon state switched to DEFAULT");
	}
}

void Control_Scene_Practice::_on_back_button_pressed() 
{
	// get the scene tree and switch back to Scene_Home via Control_Main
	UtilityFunctions::print("Back button pressed, returning to Scene_Home");
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

	// stop related physics processing before scene switch to avoid accessing detached nodes.
	set_physics_process(false);
	if (dragon_control && dragon_control->is_inside_tree())
	{
		dragon_control->set_physics_process(false);
	}
	if (ctrl_camera && ctrl_camera->is_inside_tree())
	{
		ctrl_camera->set_physics_process(false);
		ctrl_camera->set_process_input(false);
	}

	// restore camera_main to its original position.
	if (camera_main && camera_main->is_inside_tree() && root) 
	{
		Node *main_node = root->get_node_or_null(NodePath("Main"));
		if (main_node) {
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

	// prefer the existing cached reference.
	if (!control_main) 
	{
		// retry lookup (supports running the sub-scene directly).
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
		UtilityFunctions::printerr("Control_Scene_Practice: Control_Main not available to switch scene. Run from Main scene to enable navigation.");
	}
}