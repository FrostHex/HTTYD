#include "DragonControlTop.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>        // memnew
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void DragonControlTop::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_joystick_input", "value"), &DragonControlTop::set_joystick_input);
    ClassDB::bind_method(D_METHOD("is_joystick_input"), &DragonControlTop::is_joystick_input);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "joystick_input"), "set_joystick_input", "is_joystick_input");
}

DragonControlTop::DragonControlTop() {
    // 默认构造
}

DragonControlTop::~DragonControlTop() {
    // 析构器
}

void DragonControlTop::_ready() {
    if (!joystick_input) {
        DragonControlKeyboard *keyboard = memnew(DragonControlKeyboard);
        add_child(keyboard);
    }
}

void DragonControlTop::set_joystick_input(bool p_val) {
    joystick_input = p_val;
}

bool DragonControlTop::is_joystick_input() const {
    return joystick_input;
}