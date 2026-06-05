#include "Control_Scene_Practice.h"
#include "Control_Main.h"
#include "Settings.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/font_file.hpp>
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
#include <godot_cpp/classes/xr_controller3d.hpp>

using namespace godot;

Control_Scene_Practice::Control_Scene_Practice() {}
Control_Scene_Practice::~Control_Scene_Practice() {}

void Control_Scene_Practice::_bind_methods() 
{
}

void Control_Scene_Practice::_ready() 
{
	if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

	Control_Scene_Top::_ready();
	
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
		int lang = Settings::GetSingleton()->GetValLanguage();
		switch (lang)
		{
			case LANGUAGE_CHINESE:
				json_file = "res://Media/Text/Chinese.json";
				break;
			case LANGUAGE_RUNIC:
				json_file = "res://Media/Text/Runic.json";
				break;
			default:
				break;
		}
	}

	if (control_main && practice_label) {
		String font_path = "res://Media/Font/Arial_Bold.ttf";
		switch (Settings::GetSingleton()->GetValLanguage())
		{
			case LANGUAGE_CHINESE:
				font_path = "res://Media/Font/Microsoft_YaHei.ttf";
				break;
			case LANGUAGE_RUNIC:
				font_path = "res://Media/Font/Rune.otf";
				break;
			default:
				break;
		}

		Ref<FontFile> practice_font;
		practice_font.instantiate();
		if (practice_font.is_valid() && practice_font->load_dynamic_font(font_path) == OK)
			practice_label->set_font(practice_font);
		else
			UtilityFunctions::printerr("Control_Scene_Practice: Failed to load font: ", font_path);
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
			String practice_key = Settings::GetSingleton()->GetValEnableHeadset()
				? String("practice_vr")
				: String("practice_nonvr");
			if (data.has(practice_key)) 
			{
				Array practice_array = data[practice_key];
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
    if (Settings::GetSingleton()->GetValEnableHeadset()) 
    {
        dragon_pilot = memnew(Dragon_Pilot_Joystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot)); // add the dragon control to the dragon node
		dragon_pilot->set_name("Dragon_Pilot_Joystick"); // set the name of the dragon control node
	}
    else
    {
        dragon_pilot = memnew(Dragon_Pilot_Keyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot));
		dragon_pilot->set_name("Dragon_Pilot_Keyboard"); // set the name of the dragon control node
    }
    ctrl_camera->SetDragon_Pilot_(dragon_pilot); // set the dragon control to the camera control
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
	if (dragon_pilot) {
		dragon_pilot->SetState(DragonState::STATE_DEFAULT);
	}

	// show Practice text (display only the first sentence).
	if (practice_label && lines.size() > 0) {
		String practice_text = lines[0];
		practice_label->set_text(practice_text);
	}
}


void Control_Scene_Practice::_input(const Ref<InputEvent> &event) 
{
    _input_top(event);
	
	if (!event.is_valid()) return;
	Ref<InputEventKey> key_event = event;
	if (key_event.is_null()) return;
	if (!key_event->is_pressed() || key_event->is_echo()) return; // 只处理按下的首次事件

	// Enter or keypad Enter: toggle dragon state.
	if (key_event->get_keycode() == Key::KEY_ENTER || key_event->get_keycode() == Key::KEY_KP_ENTER) {
		_toggle_dragon_state();
		return;
	}
}

void Control_Scene_Practice::_process(double delta)
{
	_process_top(delta);
}

void Control_Scene_Practice::_physics_process(double delta)
{
	// VR controller A/B button logic, following Tutorial.cpp.
	// enable only when the right controller exists and dragon control is in VR mode.
	if (hand_right && control_main && Settings::GetSingleton()->GetValEnableHeadset()) 
	{
		// A/B are right-hand buttons; Dragon_Pilot_Joystick uses "ax_button" / "by_button".
		float a_val = hand_right->get_float("ax_button");
		float b_val = hand_right->get_float("by_button");
		bool a_now = a_val > 0.5f;
		bool b_now = b_val > 0.5f;

		// edge-triggered: A returns home, B toggles state.
		if (a_now && !a_button_prev) 
		{
			ReturnHome();
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
	if (!dragon_pilot) return;

	// toggle state.
	is_crisis_state = !is_crisis_state;
	
	if (is_crisis_state) 
	{
		// set to crisis state.
		dragon_pilot->SetState(DragonState::STATE_CRISIS);
		UtilityFunctions::print("Dragon state switched to CRISIS");
	} 
	else 
	{
		// set to default state.
		dragon_pilot->SetState(DragonState::STATE_DEFAULT);
		UtilityFunctions::print("Dragon state switched to DEFAULT");
	}
}
