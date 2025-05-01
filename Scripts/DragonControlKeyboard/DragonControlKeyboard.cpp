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

#if 0  // disable old physics override, now handled by base class
/**
 * @brief called every frame
 * @param delta time since last frame
 */
void DragonControlKeyboard::_physics_process(double delta) 
{
    // get attached rigid body (parent node)
    RigidBody3D *dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (!dragon_rb) 
    {
        UtilityFunctions::printerr("DragonControlKeyboard: parent is not RigidBody3D");
        return;
    }
    
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
    linear_velocity = linear_velocity_input + std::copysign(1.0f, height_delta) * std::sqrt(19.6 * std::abs(height_delta));
    
    // minimum speed enforcement
    if (linear_velocity < 3.0) 
    {
        linear_velocity_input += 3.0 - linear_velocity;
        linear_velocity = 3.0;
    }
    // linear speed control
    Vector3 fwd = dragon_rb->get_global_transform().basis.get_column(0);
    dragon_rb->set_linear_velocity(fwd * float(linear_velocity));

    // angular speed control)
    Basis basis = dragon_rb->get_global_transform().basis;
    Vector3 angular_velocity = Vector3(0, 0, 0);
    if (input_singleton->is_key_pressed(Key::KEY_UP)) 
    {
        angular_velocity_buildup -= basis.get_column(2) * DRAGON_FACTOR_PITCH;
    } 
    else if (input_singleton->is_key_pressed(Key::KEY_DOWN))
    {
        angular_velocity_buildup += basis.get_column(2) * DRAGON_FACTOR_PITCH;
    }
    if (input_singleton->is_key_pressed(Key::KEY_LEFT)) 
    {
        angular_velocity_buildup -= basis.get_column(0) * DRAGON_FACTOR_ROLL; // roll left
    } 
    else if (input_singleton->is_key_pressed(Key::KEY_RIGHT)) 
    {
        angular_velocity_buildup += basis.get_column(0) * DRAGON_FACTOR_ROLL; // roll right
    }
    angular_velocity_buildup = angular_velocity_buildup * DRAGON_FACTOR_DAMPING; // damping

    // coupling yaw with roll angle
    Vector3 right = basis.get_column(2); // local z-axis(right)
    float tilt  = right.dot(Vector3(0,1,0)); // dot product with global y-axis(up)
    tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt); // clamp to [-1, 1]
    float roll_angle = std::asin(tilt); // arcsin to get the roll angle (radians)
    Vector3 angular_velocity_posture = Vector3(0, 1, 0) * roll_angle * DRAGON_FACTOR_YAW;

    // additionally change pitch when upside down
    Vector3 up = basis.get_column(1); // local y-axis(up)
    tilt = up.dot(Vector3(0,1,0)); // dot product with global y-axis(up)
    if (tilt < 0.0f) // if the dragon is upside down
    {
        tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt); // clamp to [-1, 1]
        angular_velocity_posture -= basis.get_column(2) * tilt * DRAGON_FACTOR_UPSIDE_DOWN;
        // UtilityFunctions::print(String("Pitch: ") + String::num(std::asin(tilt) * 180.0 / Math_PI));
    }

    angular_velocity = angular_velocity_buildup + angular_velocity_posture; // combine to get the final angular velocity
    dragon_rb->set_angular_velocity(angular_velocity);



    // UtilityFunctions::print(1/delta); // print the frame rate
    // UtilityFunctions::print(typeid(DRAGON_FACTOR_PITCH).name());
    // UtilityFunctions::print(angular_velocity_buildup); // print the current angular velocity
    // UtilityFunctions::print(linear_velocity); // print the current linear velocity
    // UtilityFunctions::print(String("Roll: ") + String::num(roll_angle * 180.0 / Math_PI));
}
#endif

// 模板基类接口实现
float DragonControlKeyboard::getLinearInput() {
    float val = 0.0f;
    if (input_singleton->is_key_pressed(Key::KEY_W)) {
        val += DRAGON_FACTOR_LINEAR;
    } else if (input_singleton->is_key_pressed(Key::KEY_S)) {
        val -= DRAGON_FACTOR_LINEAR;
    }
    return val;
}

void DragonControlKeyboard::handleAngular(RigidBody3D *dragon_rb) {
    Basis basis = dragon_rb->get_global_transform().basis;
    if (input_singleton->is_key_pressed(Key::KEY_UP)) {
        angular_velocity_buildup -= basis.get_column(2) * DRAGON_FACTOR_PITCH;
    } else if (input_singleton->is_key_pressed(Key::KEY_DOWN)) {
        angular_velocity_buildup += basis.get_column(2) * DRAGON_FACTOR_PITCH;
    }
    if (input_singleton->is_key_pressed(Key::KEY_LEFT)) {
        angular_velocity_buildup -= basis.get_column(0) * DRAGON_FACTOR_ROLL;
    } else if (input_singleton->is_key_pressed(Key::KEY_RIGHT)) {
        angular_velocity_buildup += basis.get_column(0) * DRAGON_FACTOR_ROLL;
    }
    angular_velocity_buildup = angular_velocity_buildup * DRAGON_FACTOR_DAMPING;
    Vector3 right = basis.get_column(2);
    float tilt = right.dot(Vector3(0,1,0));
    tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt);
    float roll_angle = std::asin(tilt);
    Vector3 angular_velocity_posture = Vector3(0,1,0) * roll_angle * DRAGON_FACTOR_YAW;
    Vector3 up = basis.get_column(1);
    tilt = up.dot(Vector3(0,1,0));
    if (tilt < 0.0f) {
        tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt);
        angular_velocity_posture -= basis.get_column(2) * tilt * DRAGON_FACTOR_UPSIDE_DOWN;
    }
    Vector3 final_ang = angular_velocity_buildup + angular_velocity_posture;
    dragon_rb->set_angular_velocity(final_ang);
}

void DragonControlKeyboard::_bind_methods() { }

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