#include "DragonControlTop.h"

#include <godot_cpp/godot.hpp> // a wrapper for the Godot C++ API
#include <godot_cpp/core/class_db.hpp>  // class registration
#include <godot_cpp/classes/input.hpp>  // DRAGON_FACTOR_LINEARess input device

using namespace godot;

DragonControlTop::DragonControlTop() 
{
    set_physics_process(true);
}

DragonControlTop::~DragonControlTop() 
{
}

/**
 * @brief bind methods and properties to the Godot engine
 * @note in order to use in GDScript or other scripts
 * @note usually call register_method() and register_property() in this function to bind methods and properties
 */
void DragonControlTop::_bind_methods() { }

void DragonControlTop::_physics_process(double delta) {
    logic.process(this, delta);
}