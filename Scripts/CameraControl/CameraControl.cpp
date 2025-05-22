#include "CameraControl.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/window.hpp>

using namespace godot;


/**
 * @brief constructor
 * @param sub_view whether to use the sub camera
 * @param dragon_control the DragonControlKeyboard instance
 */
CameraControl::CameraControl(bool sub_view, bool enable_headset, DragonControlTop* dragon_control) 
{
    this->sub_view = sub_view;
    this->enable_headset = enable_headset;
    this->dragon_control = dragon_control;
}


/**
 * @brief destructor
 */
CameraControl::~CameraControl() 
{
}


void CameraControl::_bind_methods() 
{
}


/**
 * @brief initialize the sub camera if the "Sub View" property is enabled
 */
void CameraControl::_ready()
{
    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (!dragon_rb) 
    {
        UtilityFunctions::printerr("CameraControl: parent is not RigidBody3D");
        return;
    }
    xr_origin = get_parent()->get_node<Node>("Pivot")->get_node<Node3D>("XROrigin");
    if (!xr_origin) 
    {
        UtilityFunctions::printerr("CameraControl: XROrigin not found");
        return;
    }

    if (!Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        set_physics_process(false);

        if (this->sub_view) 
        {
            camera_sub = get_parent()->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<Camera3D>("CameraSub");
            if (!camera_sub) 
            {
                UtilityFunctions::printerr("CameraControl: Sub Camera3D not found");
                return;
            }
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0)); // set camera rotation
        
            label_info = get_parent()->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<Label>("Info");
            if (!label_info) 
            {
                UtilityFunctions::printerr("CameraControl: Label not found");
                return;
            }

            if (!dragon_control) 
            {
                UtilityFunctions::printerr("CameraControl: DragonControlKeyboard not found");
                return;
            }
            set_physics_process(true);
        } 
        else 
        {
            // Destroy the SubViewportContainer and all its children
            if (Node *container = get_parent()->get_node<Node>("SubViewportContainer")) 
            {
                container->queue_free();
            } 
            else 
            {
                UtilityFunctions::printerr("CameraControl: SubViewportContainer not found");
            }
            return;
        }

        if (enable_headset) 
        {
            set_physics_process(true);
        }
    }
}


/**
 * @brief let the sub camera follow the dragon
 */
void CameraControl::_physics_process(double delta) 
{
    if (this->enable_headset) 
    {
        xr_origin->set_global_rotation(Vector3(0, - Math_PI / 2, 0)); // set camera rotation
    }
    if (this->sub_view)
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        String velocity_text = "Linear Velocity: " + String::num(dragon_control->GetLinearVelocity(), 1); // keep 1 decimal place
        label_info->set_text(velocity_text);
    }
}