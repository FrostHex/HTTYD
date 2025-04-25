#include "DragonControlKeyboard.h"

#include <godot_cpp/godot.hpp> // a wrapper for the Godot C++ API
#include <godot_cpp/core/class_db.hpp>  // class registration
#include <godot_cpp/classes/input.hpp>  // access input device
#include <godot_cpp/classes/input_event.hpp> // input event
#include <godot_cpp/variant/utility_functions.hpp> // used for printing info

using namespace godot;

/**
 * @brief constructor
 */
DragonControlKeyboard::DragonControlKeyboard() 
{
    input_singleton = Input::get_singleton();
    set_process_input(true);
    set_process(false);
}

/**
 * @brief destructor
 */
DragonControlKeyboard::~DragonControlKeyboard() 
{
}

/**
 * @brief bind methods and properties to the Godot engine
 * @note in order to use in GDScript or other scripts
 * @note usually call register_method() and register_property() in this function to bind methods and properties
 */
void DragonControlKeyboard::_bind_methods() 
{
}

/**
 * @brief called every frame
 * @param delta time since last frame
 */
void DragonControlKeyboard::_process(double delta) 
{
}

/**
 * @brief called when an input event occurs
 * @param event the input event
 */
void DragonControlKeyboard::_input(const Ref<InputEvent> &event) 
{
    if (event->is_action_pressed("ui_select")) // can be customized in Project Settings -> Input Map
    {
        UtilityFunctions::print("Spacebar is pressed!");
    }
}