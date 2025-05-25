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


/**
 * @brief constructor
 * @note use initialization list to initialize member variable state_current to STATE_DEFAULT
 */
DragonControlTop::DragonControlTop() : state_current(STATE_DEFAULT)
{
    set_physics_process(true);
    
    // initialize the state process function pointers in the array
    state_process_funcs[STATE_DEFAULT] = &DragonControlTop::ProcessDefault;
    state_process_funcs[STATE_HIT_CLIFF] = &DragonControlTop::ProcessHitCliff;
    state_process_funcs[STATE_FALLING] = &DragonControlTop::ProcessFalling;
    state_process_funcs[STATE_CRISIS] = &DragonControlTop::ProcessCrisis;
    state_process_funcs[STATE_DISABLED] = &DragonControlTop::ProcessDisabled;
}


/**
 * @brief destructor
 */
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
    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &DragonControlTop::_on_body_entered);
    ClassDB::bind_method(D_METHOD("get_state"), &DragonControlTop::GetState);
    ClassDB::bind_method(D_METHOD("set_state", "state_new"), &DragonControlTop::SetState);
    
    // use godot macro (BIND_ENUM_CONSTANT) to bind enum values to the engine
    BIND_ENUM_CONSTANT(STATE_DEFAULT);
    BIND_ENUM_CONSTANT(STATE_HIT_CLIFF);
    BIND_ENUM_CONSTANT(STATE_FALLING);
    BIND_ENUM_CONSTANT(STATE_CRISIS);
    BIND_ENUM_CONSTANT(STATE_DISABLED);
}


/**
 * @brief get rigid body and animator node, set up initial properties, and enable contact monitoring
 * @note called when the node and its children are initialized
 */
void DragonControlTop::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }

    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    dragon_animator = get_parent()->get_node<DragonAnimator>("DragonAnimator");
    dragon_rb->set_gravity_scale(0); // disable gravity
    height_init = dragon_rb->get_global_transform().origin.y;
    dragon_rb->set_contact_monitor(true); // enable contact monitoring and reporting
    dragon_rb->set_max_contacts_reported(1); // set the maximum number of contacts reported to 1
    dragon_rb->connect("body_entered", Callable(this, "_on_body_entered")); // connect the signal to the function
}


/**
 * @brief called every physics frame
 * @param delta time since last frame
 * @note call the state processing function from the pointer array (state_process_funcs)
 */
void DragonControlTop::_physics_process(double delta) 
{
    (this->*state_process_funcs[state_current])(delta);
}


/**
 * @brief the default state processing function
 * @param delta time since last frame
 */
void DragonControlTop::ProcessDefault(double delta)
{
    GetInput(this->input_keys);
    SetMotionLinear(delta);
    SetMotionAngular(delta);
    SetAnimation();
    // UtilityFunctions::print(delta);
    // UtilityFunctions::print("input_keys: ", input_keys[0], ", ", input_keys[1], ", ", input_keys[2]);
    // UtilityFunctions::print(dragon_rb->get_global_transform().origin.y);
    // UtilityFunctions::print("linear_velocity: ", linear_velocity);
}


/**
 * @brief the cliff hit state processing function
 * @param delta time since last frame
 */
void DragonControlTop::ProcessHitCliff(double delta)
{
    // TODO
}


/**
 * @brief the falling state processing function
 * @param delta time since last frame
 */
void DragonControlTop::ProcessFalling(double delta)
{
    // TODO
}


/**
 * @brief the crisis state processing function
 * @param delta time since last frame
 */
void DragonControlTop::ProcessCrisis(double delta)
{
    // TODO
}


/**
 * @brief the disabled state processing function
 * @param delta time since last frame
 */
void DragonControlTop::ProcessDisabled(double delta)
{
}


/**
 * @brief setter for the dragon state
 * @param state_new 
 */
void DragonControlTop::SetState(DragonState state_new)
{
    state_current = state_new;
    UtilityFunctions::print("Dragon state changed to: ", state_current);
}

/**
 * @brief getter for the dragon state
 * @return current state of the dragon
 */
DragonState DragonControlTop::GetState() const
{
    return state_current;
}


/**
 * @brief set linear velocity based on input keys and height difference
 * @param delta time since last frame
 * @note the minimum linear velocity is 3.0f
 */
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


/**
 * @brief set angular velocity based on input keys and dragon posture
 * @param delta time since last frame
 * @note roll and yaw are coupled
 * @note when flying upside down, the dragon will tend to point its head towards the ground
 */
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
        Vector3 axis = basis.get_column(2); // local right vector
        axis.y = 0.0f; // project to xz plane
        if (axis.length() > 0.0f) 
        {
            axis = axis.normalized();
        }
        angular_velocity_posture -= axis * std::asin(tilt) * DRAGON_FACTOR_UPSIDE_DOWN;
    }
    dragon_rb->set_angular_velocity(angular_velocity_buildup + angular_velocity_posture);
}


/**
 * @brief set animation based on dragon posture and input keys
 */
void DragonControlTop::SetAnimation() 
{
    Basis basis = dragon_rb->get_global_transform().basis;
    float tilt = basis.get_column(0).dot(Vector3(0,1,0)); // local forward vector dot global up vector
    
    if (tilt > DRAGON_FACTOR_GLIDE)    
    {
        dragon_animator->SetAnimation("wing_main", "lo_up");
    }
    else if (tilt < -2 * DRAGON_FACTOR_GLIDE)
    {
        dragon_animator->SetAnimation("wing_main", "po_dive");
    }
    else
    {
        if (input_keys[2] > 0.0f)
        {
            dragon_animator->SetAnimation("wing_main", "po_right");
        }
        else if (input_keys[2] < 0.0f)
        {
            dragon_animator->SetAnimation("wing_main", "po_left");
        }
        else
        {
            dragon_animator->SetAnimation("wing_main", "po_glide");
        }
    }

    if (input_keys[2] > 0.0f)
    {
        dragon_animator->SetAnimation("wing_tail", "po_right");
    }
    else if (input_keys[2] < 0.0f)
    {
        dragon_animator->SetAnimation("wing_tail", "po_left");
    }
    else
    {
        dragon_animator->SetAnimation("wing_tail", "po_glide");
    }
}


/**
 * @brief the getter for linear velocity
 * @return the linear velocity of the dragon
 */
float DragonControlTop::GetLinearVelocity()
{
    return linear_velocity;
}


/**
 * @brief the function to be called when a body enters the dragon's collision shape
 * @param body the body that entered the collision shape
 * @note this function is connected to the signal "body_entered" of the RigidBody3D node
 */
void DragonControlTop::_on_body_entered(Node* body)
{
    UtilityFunctions::print("COLLISION DETECTED with: ", body->get_name(), " at velocity: ", linear_velocity);
}