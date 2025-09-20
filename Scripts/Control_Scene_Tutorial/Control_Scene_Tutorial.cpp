#include "Control_Scene_Tutorial.h"
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

using namespace godot;

Control_Scene_Tutorial::Control_Scene_Tutorial() {}
Control_Scene_Tutorial::~Control_Scene_Tutorial() {}

void Control_Scene_Tutorial::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("_on_timer_timeout"), &Control_Scene_Tutorial::_on_timer_timeout);
    ClassDB::bind_method(D_METHOD("_on_back_button_pressed"), &Control_Scene_Tutorial::_on_back_button_pressed);
}

void Control_Scene_Tutorial::_ready() 
{
	SceneTree *tree = get_tree();
	if (tree) {
		Window *root = tree->get_root();
		if (root) {
			control_main = Object::cast_to<Control_Main>(root->get_node_or_null(NodePath("Main/Control_Main")));
			if (!control_main) 
			{
				Node *cand = root->find_child("Control_Main", /*recursive*/ true, /*owned*/ false);
				control_main = Object::cast_to<Control_Main>(cand);
			}
			if (!control_main) 
			{
				UtilityFunctions::printerr("Control_Scene_Tutorial: Control_Main not found. If you ran Scene_Tutorial directly, please run the project Main scene.");
			}
		}
	}

	// 获取3D文字标签引用
	Node *parent_node = get_parent();
	if (parent_node) {
		tutorial_label = Object::cast_to<Label3D>(parent_node->get_node_or_null(NodePath("TutorialPaper/TutorialText")));
		if (!tutorial_label) {
			UtilityFunctions::printerr("Control_Scene_Tutorial: Could not find TutorialText label");
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

	// 如果当前语言为中文，则将教程文本字体设置为系统黑体（含常见中文字体回退）
	if (control_main && tutorial_label) {
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
			tutorial_label->set_font(zh_font);
		}
	}

	// 使用JSON格式加载tutorial数组
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

	// 立即显示第一句话（current_index 表示当前显示的索引，保持为 0）
	if (tutorial_label && lines.size() > 0) {
		tutorial_label->set_text(lines[0]);
		current_index = 0;
	}

	// 不再使用计时器自动翻页
	// 如果未来需要恢复自动播放，可重新启用计时器逻辑
    
	// Connect back button safely
	Node *back_node = get_parent()->get_node_or_null(NodePath("UI/Button_Back"));
	if (back_node) {
		if (Button *back_button = Object::cast_to<Button>(back_node)) {
			back_button->connect("pressed", callable_mp(this, &Control_Scene_Tutorial::_on_back_button_pressed));
		}
	}
}

void Control_Scene_Tutorial::_on_timer_timeout() {
	// 已弃用：计时器不再用于翻页
}

void Control_Scene_Tutorial::_input(const Ref<InputEvent> &event) {
	if (!event.is_valid()) return;
	Ref<InputEventKey> key_event = event;
	if (key_event.is_null()) return;
	if (!key_event->is_pressed() || key_event->is_echo()) return; // 只处理按下的首次事件

	// 向右：下一页
	if (key_event->is_action_pressed("ui_right")) {
		if (lines.size() == 0 || !tutorial_label) return;
		// 边界保护：在最后一句时按右键无效果
		if (current_index >= lines.size() - 1) {
			return;
		}
		current_index += 1;
		tutorial_label->set_text(lines[current_index]);
		return;
	}

	// 向左：上一页
	if (key_event->is_action_pressed("ui_left")) {
		if (lines.size() == 0 || !tutorial_label) return;
		// 边界保护：在第一句时按左键无效果
		if (current_index <= 0) {
			return;
		}
		current_index -= 1;
		tutorial_label->set_text(lines[current_index]);
		return;
	}
}

void Control_Scene_Tutorial::_on_back_button_pressed() {
	// Get the scene tree and switch back to Scene_Home via Control_Main
	UtilityFunctions::print("Back button pressed, returning to Scene_Home");
	SceneTree *tree = get_tree();
	if (!tree) {
		UtilityFunctions::printerr("SceneTree not available");
		return;
	}
	Window *root = tree->get_root();
	if (!root) {
		UtilityFunctions::printerr("Root window not available");
		return;
	}
	// 优先使用已有缓存引用
	if (!control_main) {
		// 再次尝试定位（支持直接运行子场景的情况）
		Node *cm = root->get_node_or_null(NodePath("Main/Control_Main"));
		if (!cm) {
			cm = root->find_child("Control_Main", /*recursive*/ true, /*owned*/ false);
		}
		control_main = Object::cast_to<Control_Main>(cm);
	}
	if (control_main) {
		control_main->call("Switch_Scene", "Scene_Home");
	} else {
		UtilityFunctions::printerr("Control_Scene_Tutorial: Control_Main not available to switch scene. Run from Main scene to enable navigation.");
	}
}

