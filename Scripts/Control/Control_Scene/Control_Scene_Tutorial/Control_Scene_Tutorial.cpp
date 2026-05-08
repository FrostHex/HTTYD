#include "Control_Scene_Tutorial.h"
#include "Control_Main.h"
#include "Settings.h"

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

Control_Scene_Tutorial::Control_Scene_Tutorial() {}
Control_Scene_Tutorial::~Control_Scene_Tutorial() {}

void Control_Scene_Tutorial::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("_on_back_button_pressed"), &Control_Scene_Tutorial::_on_back_button_pressed);
}

void Control_Scene_Tutorial::_ready() 
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

	// get the 3D text label reference.
	Node *parent_node = get_parent();
	if (parent_node) {
		tutorial_label = Object::cast_to<Label3D>(parent_node->get_node_or_null(NodePath("TutorialPaper/TutorialText")));
		if (!tutorial_label) {
			UtilityFunctions::printerr("Control_Scene_Tutorial: Could not find TutorialText label");
		}
	}

	String json_file = "res://Media/Text/English.json";
	if (control_main) 
	{
		int lang = Settings::GetSingleton()->GetValLanguage();
		if (lang == 1) 
		{
			json_file = "res://Media/Text/Chinese.json";
		}
	}

	// if the current language is Chinese, set tutorial text font to a system sans-serif Chinese fallback stack.
	if (control_main && tutorial_label) {
		if (Settings::GetSingleton()->GetValLanguage() == 1) 
		{
			Ref<SystemFont> zh_font;
			zh_font.instantiate();
			PackedStringArray font_names;
			// common Chinese font priority on Windows.
			font_names.push_back("SimHei");
			font_names.push_back("Microsoft YaHei");
			zh_font->set_font_names(font_names);
			// apply to the 3D label.
			tutorial_label->set_font(zh_font);
		}
	}

	// load the tutorial array using JSON.
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
			if (data.has("tutorial")) 
			{
				Array tutorial_array = data["tutorial"];
				for (int i = 0; i < tutorial_array.size(); i++) 
				{
					lines.push_back(String(tutorial_array[i]));
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
    
	// Connect back button safely
	Node *back_node = get_parent()->get_node_or_null(NodePath("UI/Button_Back"));
	if (back_node) {
		if (Button *back_button = Object::cast_to<Button>(back_node)) {
			back_button->connect("pressed", callable_mp(this, &Control_Scene_Tutorial::_on_back_button_pressed));
		}
	}

	// Set the connection with dragon
	Node *dragon_node = get_parent()->get_node<Node>("Dragon");
    dragon_animator = get_parent()->get_node<Node>("Dragon")->get_node<Dragon_Animator>("Dragon_Animator");
	tutorial_paper = get_parent()->get_node<Node3D>("TutorialPaper");
	tutorial_paper->call_deferred("reparent", dragon_node);
	ctrl_camera = tree->get_root()->get_node<Control_Camera>("Main/Control_Main/Control_Camera");
	ctrl_camera->call_deferred("Initialize");
    if (Settings::GetSingleton()->GetValEnableHeadset()) 
    {
        dragon_pilot = memnew(Dragon_Pilot_Joystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot)); // add the dragon control to the dragon node
    }
    else
    {
        dragon_pilot = memnew(Dragon_Pilot_Keyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot));
    }
    ctrl_camera->SetDragon_Pilot_(dragon_pilot); // set the dragon control to the camera control
	dragon_node->call_deferred("set_rotation", Vector3(0.0f, 0.0f, 0.05f));

	hand_right = nullptr;
	Node *xr_origin = nullptr;
	camera_main = get_tree()->get_root()->get_node<Node3D>("Main/Camera_Main");
	if (camera_main) 
	{
		Node *xr_node = camera_main->get_node_or_null(NodePath("XR"));
		if (xr_node) 
		{
			xr_origin = xr_node->get_node_or_null(NodePath("XROrigin"));
		}
	}

	if (xr_origin) 
	{
		hand_right = Object::cast_to<XRController3D>(xr_origin->get_node_or_null(NodePath("RightHand")));
	}

	// display the first sentence immediately (current_index remains 0).
	if (tutorial_label && lines.size() > 0) {
		current_index = 0;
		String processed_text = _process_tutorial_text(lines[0]);
		tutorial_label->set_text(processed_text);
	}
}

void Control_Scene_Tutorial::_goto_next_line() {
	if (!tutorial_label || lines.size() == 0) return;
	if (current_index >= lines.size() - 1) return; // 已是最后一句
	current_index += 1;
	String processed_text = _process_tutorial_text(lines[current_index]);
	tutorial_label->set_text(processed_text);
}

void Control_Scene_Tutorial::_goto_prev_line() {
	if (!tutorial_label || lines.size() == 0) return;
	if (current_index <= 0) return; // 已是第一句
	current_index -= 1;
	String processed_text = _process_tutorial_text(lines[current_index]);
	tutorial_label->set_text(processed_text);
}

String Control_Scene_Tutorial::_process_tutorial_text(const String &text) 
{
	// check whether it starts with "/!".
	if (text.length() >= 3 && text.substr(0, 2) == "/!") 
	{
		tutorial_paper->set_position(Vector3(0.85f, 0.48f, 0.4f));
		if (dragon_animator)
		{
			dragon_animator->call_deferred("SetAnimation_Weight", "add_shake", 1.0f);
		}
		char state_char = text[2];
		if (state_char == '1') 
		{
			// set dragon state to default.
			if (dragon_pilot) 
			{
				dragon_pilot->SetState(DragonState::STATE_DEFAULT);
				dragon_pilot->set_physics_process(true);
			}
			return text.substr(3); // return text after removing the first three characters.
		}
		else if (state_char == '2') 
		{
			// set dragon state to crisis mode.
			if (dragon_pilot) 
			{
				dragon_pilot->SetState(DragonState::STATE_CRISIS);
				dragon_pilot->set_physics_process(true);
			}
			return text.substr(3);
		}
	}
	if (dragon_pilot) 	// fallback on mismatch: set disabled state.
	{
		if (Math::abs(dragon_pilot->GetLinearVelocity()) > 0.01f) 
		{
			dragon_animator->call_deferred("SetAnimation", "layer_wing_main", "lo_up");
		}
		dragon_animator->call_deferred("SetAnimation_Weight", "add_shake", 0.0f);
		dragon_pilot->call_deferred("SetState", DragonState::STATE_DISABLED);
		dragon_pilot->call_deferred("SetState", DragonState::STATE_DISABLED);
		dragon_pilot->call_deferred("SetVelocityAngular", Vector3(0.0f, 0.0f, 0.0f));
		tutorial_paper->call_deferred("set_position", Vector3(0.8f, 0.65f, 0.0f));
	}
	return text;
}

void Control_Scene_Tutorial::_input(const Ref<InputEvent> &event) 
{
	if (!event.is_valid()) return;
	Ref<InputEventKey> key_event = event;
	if (key_event.is_null()) return;
	if (!key_event->is_pressed() || key_event->is_echo()) return; // 只处理按下的首次事件

	// Enter or keypad Enter: next page.
	if (key_event->get_keycode() == Key::KEY_ENTER || key_event->get_keycode() == Key::KEY_KP_ENTER) {
		_goto_next_line();
		return;
	}

	// Backspace: previous page.
	if (key_event->get_keycode() == Key::KEY_BACKSPACE) {
		_goto_prev_line();
		return;
	}
}

void Control_Scene_Tutorial::_physics_process(double delta)
{
	// keep tutorial-page logic polling the VR controller every frame without affecting keyboard input.
	// enable only when the right controller exists and dragon control is in VR mode.
	if (hand_right && control_main && Settings::GetSingleton()->GetValEnableHeadset()) 
	{
		// A/B are right-hand buttons; Dragon_Pilot_Joystick uses "ax_button" / "by_button".
		float a_val = hand_right->get_float("ax_button");
		float b_val = hand_right->get_float("by_button");
		bool a_now = a_val > 0.5f;
		bool b_now = b_val > 0.5f;

		// edge-triggered: A previous page, B next page.
		if (b_now && !b_button_prev) 
		{
			// in VR mode: if already on the last page, pressing B again returns to home.
			if (tutorial_label && lines.size() > 0 && current_index >= lines.size() - 1) 
			{
				call_deferred("_on_back_button_pressed");
			} 
			else 
			{
				_goto_next_line();
			}
		}
		if (a_now && !a_button_prev) 
		{
			_goto_prev_line();
		}
		a_button_prev = a_now;
		b_button_prev = b_now;
	}
}

void Control_Scene_Tutorial::_on_back_button_pressed() 
{
	// Get the scene tree and switch back to Scene_Home via Control_Main
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
		UtilityFunctions::printerr("Control_Scene_Tutorial: Control_Main not available to switch scene. Run from Main scene to enable navigation.");
	}
}