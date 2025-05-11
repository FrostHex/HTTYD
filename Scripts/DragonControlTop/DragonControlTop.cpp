#include "DragonControlTop.h"

#include <godot_cpp/godot.hpp> // a wrapper for the Godot C++ API
#include <godot_cpp/core/class_db.hpp> // class registration
#include <godot_cpp/classes/input.hpp> // the input device
#include <godot_cpp/classes/engine.hpp> // Engine class for checking editor hint
#include <godot_cpp/variant/utility_functions.hpp> // used for printing info
#include <godot_cpp/classes/rigid_body3d.hpp> // RigidBody3D for physics control
#include <godot_cpp/variant/vector3.hpp> // Vector3 for velocity and position
#include <cmath> // std::copysign, std::sqrt

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
void DragonControlTop::_bind_methods()
{
}

void DragonControlTop::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }

    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (!dragon_rb) 
    {
        UtilityFunctions::printerr("DragonControlTop: parent not RigidBody3D");
        return;
    }

    dragon_animator = get_parent()->get_node<DragonAnimator>("DragonAnimator");
    if (!dragon_animator)
    {
        UtilityFunctions::printerr("DragonControlTop: DragonAnimator not found");
        return;
    }

    dragon_rb->set_gravity_scale(0); // disable gravity
    height_init = dragon_rb->get_global_transform().origin.y;
}

void DragonControlTop::_physics_process(double delta) 
{
    GetInput(this->input_keys);
    SetMotionLinear(delta);
    SetMotionAngular(delta);
    // UtilityFunctions::print(delta);
    // UtilityFunctions::print("input_keys: ", input_keys[0], ", ", input_keys[1], ", ", input_keys[2]);
    // UtilityFunctions::print(dragon_rb->get_global_transform().origin.y);
    // UtilityFunctions::print("linear_velocity: ", linear_velocity);
}

void DragonControlTop::SetMotionLinear(double delta) 
{
    linear_velocity_input += this->input_keys[0] * DRAGON_FACTOR_LINEAR * delta;
    height_delta = height_init - dragon_rb->get_global_transform().origin.y;
    linear_velocity = linear_velocity_input + std::copysign(1.0f, height_delta) * std::sqrt(19.6f * std::abs(height_delta));
    if (linear_velocity < 3.0f) 
    {
        linear_velocity_input += 3.0f - linear_velocity;
        linear_velocity = 3.0f;
    }
    Vector3 fwd = dragon_rb->get_global_transform().basis.get_column(0);
    dragon_rb->set_linear_velocity(fwd * linear_velocity);
}

void DragonControlTop::SetMotionAngular(double delta) 
{
    Basis basis = dragon_rb->get_global_transform().basis;
    angular_velocity_buildup += basis.get_column(2) * this->input_keys[1] * DRAGON_FACTOR_PITCH * delta
                              + basis.get_column(0) * this->input_keys[2] * DRAGON_FACTOR_ROLL * delta;
    angular_velocity_buildup *= DRAGON_FACTOR_DAMPING;
    float tilt = basis.get_column(2).dot(Vector3(0,1,0)); // local right vector dot global up vector
    tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt);
    Vector3 angular_velocity_posture = Vector3(0,1,0) * std::asin(tilt) * DRAGON_FACTOR_YAW * delta; // std::asin(tilt) is roll angle
    tilt = basis.get_column(1).dot(Vector3(0,1,0)); // local up vector dot global up vector
    if (tilt < 0.0f) 
    {
        tilt = tilt < -1.0f ? -1.0f : (tilt > 1.0f ? 1.0f : tilt);
        angular_velocity_posture -= basis.get_column(2) * tilt * DRAGON_FACTOR_UPSIDE_DOWN;
    }
    dragon_rb->set_angular_velocity(angular_velocity_buildup + angular_velocity_posture);

    if (input_keys[1] > 0.0f)
    {
        dragon_animator->SetAnimation("base", "lo_up");
    }
    else if (input_keys[1] < 0.0f)
    {
        dragon_animator->SetAnimation("base", "po_dive");
    }
    
}