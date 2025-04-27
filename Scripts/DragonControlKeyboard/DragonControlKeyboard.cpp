#include "DragonControlKeyboard.h"

#include <godot_cpp/godot.hpp> // a wrapper for the Godot C++ API
#include <godot_cpp/core/class_db.hpp>  // class registration
#include <godot_cpp/classes/input.hpp>  // DRAGON_FACTOR_LINEARess input device
#include <godot_cpp/classes/engine.hpp> // Engine class for checking editor hint
#include <godot_cpp/classes/input_event.hpp> // input event
#include <godot_cpp/variant/utility_functions.hpp> // used for printing info
#include <godot_cpp/classes/rigid_body3d.hpp>  // RigidBody3D for physics control
#include <godot_cpp/variant/vector3.hpp>      // Vector3 for velocity
#include <godot_cpp/classes/character_body3d.hpp>  // CharacterBody3D for physics control
#include <cmath>

using namespace godot;

/**
 * @brief constructor
 */
DragonControlKeyboard::DragonControlKeyboard() 
{
    input_singleton = Input::get_singleton();
    set_process_input(true);
    set_physics_process(true);
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
void DragonControlKeyboard::_physics_process(double delta) 
{
    // get attached rigid body (parent node)
    RigidBody3D *dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (!dragon_rb) {
        UtilityFunctions::printerr("DragonControlKeyboard: parent is not RigidBody3D");
        return;
    }
    
    // linear speed control: replace basic movement with DRAGON_FACTOR_LINEAReleration and gravity conversion
    // initialize height reference
    if (!height_initialized) 
    {
        height_init = dragon_rb->get_global_transform().origin.y;
        height_initialized = true;
    }

    if (input_singleton->is_key_pressed(Key::KEY_W)) 
    {
        linear_velocity_input += DRAGON_FACTOR_LINEAR * delta;
    } 
    else if (input_singleton->is_key_pressed(Key::KEY_S)) 
    {
        linear_velocity_input -= DRAGON_FACTOR_LINEAR * delta;
    }

    // gravity potential energy to kinetic energy conversion
    height_delta = height_init - dragon_rb->get_global_transform().origin.y;
    linear_velocity = linear_velocity_input + std::copysign(1.0, height_delta) * std::sqrt(19.6 * std::abs(height_delta));
    // minimum speed enforcement
    if (linear_velocity < 3.0) 
    {
        linear_velocity_input += 3.0 - linear_velocity;
        linear_velocity = 3.0;
    }
    // apply forward velocity
    Vector3 fwd = dragon_rb->get_global_transform().basis.get_column(2);
    dragon_rb->set_linear_velocity(fwd * float(linear_velocity));
    // angular speed control: reset and DRAGON_FACTOR_LINEARumulate pitch and roll (local coords)
    Basis basis = dragon_rb->get_global_transform().basis;
    if (input_singleton->is_key_pressed(Key::KEY_UP)) 
    {
        angular_velocity += basis.get_column(0) * DRAGON_FACTOR_PITCH;
    } 
    else if (input_singleton->is_key_pressed(Key::KEY_DOWN))
    {
        angular_velocity -= basis.get_column(0) * DRAGON_FACTOR_PITCH;
    }
    if (input_singleton->is_key_pressed(Key::KEY_LEFT)) 
    {
        angular_velocity -= basis.get_column(2) * DRAGON_FACTOR_ROLL; // roll left
    } 
    else if (input_singleton->is_key_pressed(Key::KEY_RIGHT)) 
    {
        angular_velocity += basis.get_column(2) * DRAGON_FACTOR_ROLL; // roll right
    }
    angular_velocity = angular_velocity * DRAGON_FACTOR_DAMPING; // damping
    dragon_rb->set_angular_velocity(angular_velocity);

    // UtilityFunctions::print(1/delta); // print the frame rate
    // UtilityFunctions::print(typeid(DRAGON_FACTOR_PITCH).name());
    // UtilityFunctions::print(angular_velocity); // print the current angular velocity
    UtilityFunctions::print(linear_velocity); // print the current linear velocity
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