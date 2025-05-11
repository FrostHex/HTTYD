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

CameraControl::CameraControl(bool sub_view)
{
    this->sub_view = sub_view;
}

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
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }

    if (this->sub_view) 
    {
        set_physics_process(true);
        camera_sub = get_parent()->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<Camera3D>("CameraSub");
        if (!camera_sub) 
        {
            UtilityFunctions::printerr("CameraControl: Sub Camera3D not found");
            return;
        }
        camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0)); // set camera rotation
    } 
    else 
    {
        set_physics_process(false);
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
        }
        return;
    }
}

/**
 * @brief let the sub camera follow the dragon
 */
void CameraControl::_physics_process(double delta) 
{
    // get attached rigid body (parent node)
    RigidBody3D *dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (!dragon_rb) 
    {
        UtilityFunctions::printerr("CameraControl: parent is not RigidBody3D");
        return;
    }
    
    camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
}