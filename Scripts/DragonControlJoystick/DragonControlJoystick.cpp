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

    // 获取XR Origin下的控制器节点
    left_hand = get_parent()->get_node<Node>("Pivot")->get_node<Node>("XROrigin3D")->get_node<XRController3D>("LeftHand");
    right_hand = get_parent()->get_node<Node>("Pivot")->get_node<Node>("XROrigin3D")->get_node<XRController3D>("RightHand");
}

void DragonControlJoystick::GetInput(float* input_keys) 
{
    // 读取并打印左右 XR 控制器的摇杆实时数值
}

void DragonControlJoystick::_bind_methods() 
{
}