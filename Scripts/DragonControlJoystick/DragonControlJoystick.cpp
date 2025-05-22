#include "DragonControlJoystick.h"

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
#include <godot_cpp/classes/xr_controller3d.hpp>

using namespace godot;


/**
 * @brief constructor
 */
DragonControlJoystick::DragonControlJoystick() 
{
}


/**
 * @brief destructor
 */
DragonControlJoystick::~DragonControlJoystick() 
{
}


/**
 * @brief initialize OpenXR interface and get the XR controllers
 * @note called when the node and its children are initialized
 */
void DragonControlJoystick::_ready()
{
    DragonControlTop::_ready();
    
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

    // get the XR controllers
    hand_left = get_parent()->get_node<Node>("Pivot")->get_node<Node>("XROrigin")->get_node<XRController3D>("LeftHand");
    hand_right = get_parent()->get_node<Node>("Pivot")->get_node<Node>("XROrigin")->get_node<XRController3D>("RightHand");
    if (!hand_left || !hand_right) 
    {
        UtilityFunctions::printerr("Failed to find XR controllers");
    }
}


/**
 * @brief get input from the joystick
 * @param input_keys array to store the input values, 0 for linear movement, 1 for pitch, 2 for yaw
 * @note the type of the value in the array is float, with the range of -1 to 1
 */
void DragonControlJoystick::GetInput(float* input_keys) 
{
        input_keys[0] = hand_left->get_float("trigger") - hand_right->get_float("trigger");
        input_keys[1] = - hand_left->get_vector2("primary").y; // hand_left->get_vector2("primary") is Vector2. ~.y is float
        input_keys[2] = hand_right->get_vector2("primary").x;
        // float left_grip = hand_left->get_float("grip"); // 0 to 1
        // float right_grip = hand_right->get_float("grip");
}

void DragonControlJoystick::_bind_methods() 
{
}