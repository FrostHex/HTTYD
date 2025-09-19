#include "Control_Scene_Tutorial.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Control_Scene_Tutorial::Control_Scene_Tutorial() {}
Control_Scene_Tutorial::~Control_Scene_Tutorial() {}

void Control_Scene_Tutorial::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_timer_timeout"), &Control_Scene_Tutorial::_on_timer_timeout);
    ClassDB::bind_method(D_METHOD("_on_back_button_pressed"), &Control_Scene_Tutorial::_on_back_button_pressed);
}

void Control_Scene_Tutorial::_ready() {
	// Load three lines from Text/English.txt by simple key=value format
	Ref<FileAccess> f = FileAccess::open("res://Text/English.txt", FileAccess::READ);
	if (f.is_valid()) {
		String l1, l2, l3;
		while (!f->eof_reached()) {
			String line = f->get_line().strip_edges();
			if (line.is_empty()) continue;
			int eq = line.find("=");
			if (eq < 0) continue;
			String key = line.substr(0, eq).strip_edges();
			String value = line.substr(eq + 1).strip_edges();
			if (key == "Tutorial_Line1") l1 = value;
			else if (key == "Tutorial_Line2") l2 = value;
			else if (key == "Tutorial_Line3") l3 = value;
		}
		if (!l1.is_empty()) lines.push_back(l1);
		if (!l2.is_empty()) lines.push_back(l2);
		if (!l3.is_empty()) lines.push_back(l3);
	}

	if (lines.is_empty()) {
		// Fallback if file missing or empty
		lines.push_back("Welcome to the tutorial.");
		lines.push_back("Move around and look at the world.");
		lines.push_back("Great! Let's begin your journey.");
	}

    // Create a timer to print lines sequentially
    timer = memnew(Timer);
    timer->set_wait_time(2.0);
    timer->set_one_shot(false);
    add_child(timer);
    timer->connect("timeout", callable_mp(this, &Control_Scene_Tutorial::_on_timer_timeout));
    timer->start();
    
	// Connect back button safely
	Node *back_node = get_parent()->get_node_or_null(NodePath("UI/Button_Back"));
	if (back_node) {
		if (Button *back_button = Object::cast_to<Button>(back_node)) {
			back_button->connect("pressed", callable_mp(this, &Control_Scene_Tutorial::_on_back_button_pressed));
		}
	}
}

void Control_Scene_Tutorial::_on_timer_timeout() {
	if (current_index < lines.size()) {
		UtilityFunctions::print(lines[current_index]);
		current_index++;
		if (current_index >= lines.size()) {
			timer->stop();
		}
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
	Node *control_main = root->get_node_or_null(NodePath("Main/Control_Main"));
	if (control_main) {
		control_main->call("Switch_Scene", "Scene_Home");
	} else {
		UtilityFunctions::printerr("Control_Main not found at path Main/Control_Main");
	}
}

