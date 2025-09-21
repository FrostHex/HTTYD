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
// 字体相关
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
                UtilityFunctions::printerr("Control_Camera: Could not find Control_Main at Main/Control_Main");
                return;
            }
        }
    }

	// 获取3D文字标签引用（Practice纸张）
	Node *parent_node = get_parent();
	if (parent_node) {
		practice_label = Object::cast_to<Label3D>(parent_node->get_node_or_null(NodePath("Dragon/PracticePaper/PracticeText")));
		if (!practice_label) {
			UtilityFunctions::printerr("Control_Scene_Practice: Could not find PracticeText label");
		}
	}

	String json_file = "res://Text/English.json";
	if (control_main) 
	{
		int lang = control_main->GetValLanguage();
		if (lang == 1) 
		{
			json_file = "res://Text/Chinese.json";
		}
	}

	// 如果当前语言为中文，则将Practice文本字体设置为系统黑体（含常见中文字体回退）
	if (control_main && practice_label) {
		if (control_main->GetValLanguage() == 1) 
		{
			Ref<SystemFont> zh_font;
			zh_font.instantiate();
			PackedStringArray font_names;
			// Windows 常见中文黑体/中文字体优先级
			font_names.push_back("SimHei");
			font_names.push_back("Microsoft YaHei");
			zh_font->set_font_names(font_names);
			// 应用到 3D 标签
			practice_label->set_font(zh_font);
		}
	}

	// 使用JSON格式加载practice数组
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
    dragon_animator = get_parent()->get_node<Node>("Dragon")->get_node<DragonAnimator>("DragonAnimator");
    camera_main = tree->get_root()->get_node<Node>("Main/Camera_Main");
	camera_main->reparent(dragon_node);
	camera_main->call_deferred("set_position", Vector3(0.0f, 0.0f, 0.0f));
	practice_paper = get_parent()->get_node<Node3D>("Dragon/PracticePaper");
	ctrl_camera = memnew(Control_Camera());
    ctrl_camera->set_name("Control_Camera"); // set the name of the camera control node
    dragon_node->add_child(ctrl_camera); // add the camera control to the dragon node
    if (control_main->GetValEnableHeadset()) 
    {
        dragon_control = memnew(DragonControlJoystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control)); // add the dragon control to the dragon node
    }
    else
    {
        dragon_control = memnew(DragonControlKeyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control));
    }
    ctrl_camera->SetDragonControl(dragon_control); // set the dragon control to the camera control
	dragon_node->call_deferred("set_rotation", Vector3(0.0f, 0.0f, 0.05f));

	// 记录右手控制器（用于B键切换状态）。参考Tutorial.cpp的实现
	hand_right = nullptr;
	Node *xr_origin = nullptr;
	// 尝试从场景树根定位
	if (tree && tree->get_root()) {
		Node *main_node = tree->get_root()->get_node_or_null(NodePath("Main"));
		if (main_node) {
			Node *cam_main = main_node->get_node_or_null(NodePath("Camera_Main"));
			if (cam_main) {
				Node *xr_node = cam_main->get_node_or_null(NodePath("XR"));
				if (xr_node) {
					xr_origin = xr_node->get_node_or_null(NodePath("XROrigin"));
				}
			}
		}
	}
	// 如果上述未找到，尝试从 Dragon 节点的相对路径查找
	if (!xr_origin && dragon_node) {
		Node *cam_main_rel = dragon_node->get_node_or_null(NodePath("Camera_Main"));
		if (cam_main_rel) {
			Node *xr_rel = cam_main_rel->get_node_or_null(NodePath("XR"));
			if (xr_rel) {
				xr_origin = xr_rel->get_node_or_null(NodePath("XROrigin"));
			}
		}
	}
	if (xr_origin) {
		hand_right = Object::cast_to<XRController3D>(xr_origin->get_node_or_null(NodePath("RightHand")));
	}

	// 初始化龙状态为默认状态
	is_crisis_state = false;
	if (dragon_control) {
		dragon_control->SetState(DragonState::STATE_DEFAULT);
	}

	// 显示Practice文本（只显示第一句话）
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

	// Enter 或小键盘回车：切换龙的状态
	if (key_event->get_keycode() == Key::KEY_ENTER || key_event->get_keycode() == Key::KEY_KP_ENTER) {
		_toggle_dragon_state();
		return;
	}

	// Backspace：返回主页
	if (key_event->get_keycode() == Key::KEY_BACKSPACE) {
		_on_back_button_pressed();
		return;
	}
}

void Control_Scene_Practice::_physics_process(double delta)
{
	// VR手柄A/B按钮逻辑，参考Tutorial.cpp
	// 仅当存在右手控制器并且龙控制为 VR 模式时才启用
	if (hand_right && control_main && control_main->GetValEnableHeadset()) 
	{
		// A/B 为右手按钮。参考 DragonControlJoystick：按钮名使用 "ax_button" / "by_button"
		float a_val = hand_right->get_float("ax_button");
		float b_val = hand_right->get_float("by_button");
		bool a_now = a_val > 0.5f;
		bool b_now = b_val > 0.5f;

		// 边沿触发：A 返回主页，B 切换状态
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

	// 切换状态
	is_crisis_state = !is_crisis_state;
	
	if (is_crisis_state) 
	{
		// 设置为危机状态
		dragon_control->SetState(DragonState::STATE_CRISIS);
		UtilityFunctions::print("Dragon state switched to CRISIS");
	} 
	else 
	{
		// 设置为默认状态
		dragon_control->SetState(DragonState::STATE_DEFAULT);
		UtilityFunctions::print("Dragon state switched to DEFAULT");
	}
}

void Control_Scene_Practice::_on_back_button_pressed() 
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

	// 将camera_main恢复到原位
	if (camera_main && root) 
	{
		Node *main_node = root->get_node_or_null(NodePath("Main"));
		if (main_node) {
			camera_main->reparent(main_node);
			camera_main->call_deferred("set_transform", Transform3D(Basis(), Vector3(0.0f, 10.0f, 0.0f)));
			Node* xr_origin = main_node->get_node_or_null(NodePath("Camera_Main/XR/XROrigin"));
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

	// 优先使用已有缓存引用
	if (!control_main) 
	{
		// 再次尝试定位（支持直接运行子场景的情况）
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