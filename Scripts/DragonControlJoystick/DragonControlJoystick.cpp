#include "DragonControlJoystick.h"

#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>

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
 * @brief called when the node and its children are initialized
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
}

void DragonControlJoystick::GetInput(float* input_keys) 
{
    // TODO
}

void DragonControlJoystick::_bind_methods() 
{
}