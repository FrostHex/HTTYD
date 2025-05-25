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
 * @param enable_headset whether to enable the XR headset
 */
CameraControl::CameraControl(bool sub_view, bool enable_headset) 
{
    this->sub_view = sub_view;
    this->enable_headset = enable_headset;
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


void CameraControl::SetDragonControl(DragonControlTop* dragon_control) 
{
    this->dragon_control = dragon_control;
}


/**
 * @brief initialize the sub camera if the "Sub View" property is enabled
 */
void CameraControl::_ready()
{
    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    xr_node = get_parent()->get_node<Node>("Pivot")->get_node<Node3D>("XR");
    xr_origin = xr_node->get_node<Node3D>("XROrigin");
    xr_camera = xr_origin->get_node<Node3D>("XRCamera");

    if (!Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        set_physics_process(false);

        if (this->enable_headset)
        {
            // Store the initial transforms to use later
            initial_origin_position = xr_origin->get_position();
            // initial_origin_rotation = xr_node->get_quaternion();
            
            // We need to wait for valid XR camera values
            xr_position_initialized = false;
            
            set_physics_process(true);
        }
        else
        {
            xr_node->queue_free(); // remove the XR node if not using headset
        }

        if (this->sub_view) 
        {
            camera_sub = get_parent()->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<Camera3D>("CameraSub");
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0)); // set camera rotation
            label_info = get_parent()->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<Label>("Info");
            set_physics_process(true);
        } 
        else 
        {
            // Destroy the SubViewportContainer and all its children
            if (Node *container = get_parent()->get_node<Node>("SubViewportContainer")) 
            {
                container->queue_free();
            }
            return;
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
        // Wait until we get a valid camera position (not 0,0,0)
        if (!xr_position_initialized)
        {            
            Vector3 camera_position = xr_camera->get_position();
            
            // Check if camera position is not zero vector
            if (camera_position.length_squared() > 0.001f) // Use a small epsilon for floating-point comparison
            {
                Quaternion camera_rotation = xr_camera->get_quaternion();
                
                UtilityFunctions::print("Camera position: ", camera_position);
                UtilityFunctions::print("Camera rotation: ", camera_rotation);

                initial_camera_rotation = camera_rotation;
                
                // Apply adjustment based on camera position/rotation to the initial origin transform
                xr_origin->set_position(initial_origin_position - camera_position);
                
                // Mark as initialized so we don't apply this correction again
                xr_position_initialized = true;
                
                UtilityFunctions::print("XR position initialized successfully");
            }        
        }
        
        xr_node->set_global_rotation(Vector3(0, -Math_PI / 2, 0)); // set the rotation of the XR origin to match the camera

    }
    if (this->sub_view)
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        String velocity_text = "Linear Velocity: " + String::num(dragon_control->GetLinearVelocity(), 1); // keep 1 decimal place
        label_info->set_text(velocity_text);
    }
}

Vector3 CameraControl::GetPostureHeadset()
{
    return xr_camera->get_global_rotation();
}