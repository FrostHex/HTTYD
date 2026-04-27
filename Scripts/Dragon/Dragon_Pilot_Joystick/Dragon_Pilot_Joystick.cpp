#include "Dragon_Pilot_Joystick.h"
#include "Control_Camera.h"

#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/godot.hpp> // for JoyAxis enum
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/xr_controller3d.hpp>

using namespace godot;


/**
 * @brief constructor
 */
Dragon_Pilot_Joystick::Dragon_Pilot_Joystick() 
{
}


/**
 * @brief destructor
 */
Dragon_Pilot_Joystick::~Dragon_Pilot_Joystick() 
{
}


void Dragon_Pilot_Joystick::RefreshXRControllers()
{
    Node3D *camera_node = nullptr;

    // Prefer resolving Camera_Main from the current species root under Dragon/SpeciesSlot.
    Node *species_slot = get_parent() ? get_parent()->get_node_or_null(NodePath("SpeciesSlot")) : nullptr;
    if (species_slot && species_slot->get_child_count() > 0)
    {
        Node *species_root = species_slot->get_child(species_slot->get_child_count() - 1);
        camera_node = species_root ? Object::cast_to<Node3D>(species_root->get_node_or_null(NodePath("Sockets/Socket_Back_Mount/Socket_Back/Camera_Main"))) : nullptr;
    }

    if (!camera_node && ctrl_camera)
    {
        camera_node = ctrl_camera->camera_main;
    }

    if (!camera_node && get_tree() && get_tree()->get_root())
    {
        camera_node = Object::cast_to<Node3D>(get_tree()->get_root()->get_node_or_null(NodePath("Main/Camera_Main")));
    }

    if (!camera_node)
    {
        hand_left = nullptr;
        hand_right = nullptr;
        return;
    }

    hand_left = Object::cast_to<XRController3D>(camera_node->get_node_or_null(NodePath("XR/XROrigin/LeftHand")));
    hand_right = Object::cast_to<XRController3D>(camera_node->get_node_or_null(NodePath("XR/XROrigin/RightHand")));
}


/**
 * @brief initialize OpenXR interface and get the XR controllers
 * @note called when the node and its children are initialized
 */
void Dragon_Pilot_Joystick::_ready()
{
    Dragon_Pilot_Top::_ready();
    
    // initialize OpenXR interface
    Ref<XRInterface> xr_interface = XRServer::get_singleton()->find_interface("OpenXR");
    if (xr_interface.is_valid() && xr_interface->is_initialized()) 
    {
        UtilityFunctions::print("OpenXR initialized successfully");
        DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED); // turn off vsync
        get_viewport()->set_use_xr(true); // set the viewport to use XR
    } 
    else 
    {
        UtilityFunctions::print("OpenXR not initialized, please check if your headset is connected");
    }

    RefreshXRControllers();
}

void Dragon_Pilot_Joystick::_physics_process(double delta)
{
    // run base state machine first
    Dragon_Pilot_Top::_physics_process(delta);

    if (!hand_left || !hand_right || !hand_left->is_inside_tree() || !hand_right->is_inside_tree())
    {
        RefreshXRControllers();
    }

    // Edge-detect Y button click on the LEFT hand only (ignore B on right hand)
    bool y_pressed = false;
    if (hand_left) 
    {
        y_pressed = (hand_left->get_float("by_button") > 0.5f); // Y on left
    }

    if (y_pressed && !y_button_prev) 
    {
        // Rising edge: trigger rolling if currently in crisis state
        if (GetState() == DragonState::STATE_CRISIS) 
        {
            SetState(DragonState::STATE_ROLLING);
        }
    }
    y_button_prev = y_pressed;
}


/**
 * @brief get input from the joystick
 * @param input_keys array to store the input values, 0 for linear movement, 1 for pitch, 2 for yaw
 * @note the type of the value in the array is float, with the range of -1 to 1
 */
void Dragon_Pilot_Joystick::GetInput(float* input_keys) 
{
        if (!hand_left || !hand_right)
        {
            RefreshXRControllers();
        }

        if (!hand_left || !hand_right)
        {
            input_keys[0] = 0.0f;
            input_keys[1] = 0.0f;
            input_keys[2] = 0.0f;
            return;
        }

        input_keys[0] = hand_left->get_float("trigger") - hand_right->get_float("trigger");
        input_keys[1] = - hand_left->get_vector2("primary").y; // hand_left->get_vector2("primary") is Vector2. ~.y is float
        input_keys[2] = hand_right->get_vector2("primary").x;
        // float left_grip = hand_left->get_float("grip"); // 0 to 1
        // float right_grip = hand_right->get_float("grip");
}


/**
 * @brief set angular velocity using headset orientation in crisis state
 * @param delta time since last frame
 */
void Dragon_Pilot_Joystick::SetMotionAngularCrisis(double delta) 
{
    Basis headset_basis = Basis::from_euler(ctrl_camera->GetPostureHeadset());
    Basis dragon_basis = dragon_rb->get_global_transform().basis;
    Vector3 headset_forward = -headset_basis.get_column(2); // +z
    Vector3 headset_up = headset_basis.get_column(1); // +y
    Vector3 dragon_forward = dragon_basis.get_column(0); // +x
    Vector3 dragon_up = dragon_basis.get_column(1); // +y
    Vector3 dragon_right = dragon_basis.get_column(2); // +z
    Vector3 angular_velocity = Vector3();
    // process pitch
    float pitch_diff = dragon_forward.y - headset_forward.y; // comparing the y component of the forward vectors
    angular_velocity -= dragon_right * (pitch_diff * DRAGON_CRISIS_P_GAIN);
    // process roll 
    // project the headset up vector onto the dragon's YOZ plane (perpendicular to the dragon's forward vector)
    Vector3 projected_up = headset_up - dragon_forward * headset_up.dot(dragon_forward);
    // calculate the roll by comparing the projected up vector with the dragon's up vector
    if (projected_up.length_squared() > 0.001f) 
    {
        projected_up.normalize();
        float roll_dot = dragon_up.dot(projected_up);
        float roll_angle = Math::acos(Math::clamp(roll_dot, -1.0f, 1.0f));
        float roll_dir = projected_up.dot(dragon_right) > 0.0f ? 1.0f : -1.0f;
        angular_velocity += dragon_forward * (roll_dir * roll_angle * DRAGON_CRISIS_P_GAIN * 0.8f);
    }
    // couple yaw and roll
    float tilt = dragon_right.dot(Vector3(0, 1, 0));
    tilt = Math::clamp(tilt, -1.0f, 1.0f);
    angular_velocity += Vector3(0, 1, 0) * (Math::asin(tilt) * DRAGON_FACTOR_YAW * 3 * delta);

    dragon_rb->set_angular_velocity(angular_velocity);
}

void Dragon_Pilot_Joystick::_bind_methods() 
{
}