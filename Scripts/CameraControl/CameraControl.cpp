#include "CameraControl.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>

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
            initial_origin_position = xr_origin->get_position();
            xr_position_initialized = false;
            set_physics_process(true);
        }
        else
        {
            xr_node->queue_free();
        }

        if (this->sub_view)
        {
            Node* sub_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node* sub_viewport = sub_container->get_node<Node>("SubViewport");
            camera_sub = sub_viewport->get_node<Camera3D>("CameraSub");
            label_info = sub_viewport->get_node<Label>("Info");
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0));

            if (this->enable_headset)
            {
                Ref<godot::ViewportTexture> vp_tex = Object::cast_to<Viewport>(sub_viewport)->get_texture();

                MeshInstance3D* sub_mesh = memnew(MeshInstance3D);
                PlaneMesh* plane = memnew(PlaneMesh);
                plane->set_size(Vector2(0.4f, 0.3f));
                sub_mesh->set_mesh(plane);

                StandardMaterial3D* mat = memnew(StandardMaterial3D);
                mat->set_texture(StandardMaterial3D::TEXTURE_ALBEDO, vp_tex);
                mat->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
                mat->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
                sub_mesh->set_material_override(mat);

                xr_camera->add_child(sub_mesh);
                sub_mesh->set_position(Vector3(0.27f, 0.213f, -0.5f));
                sub_mesh->set_rotation(Vector3(Math_PI / 2, 0, 0));
            }

            set_physics_process(true);
        } 
        else 
        {
            // destroy the SubViewportContainer and all its children
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
        // wait until we get a valid camera position (not 0,0,0)
        if (!xr_position_initialized)
        {            
            Vector3 camera_position = xr_camera->get_position();
            
            // check if camera position is not zero vector
            if (camera_position.length_squared() > 0.001f) // Use a small epsilon for floating-point comparison
            {
                Quaternion camera_rotation = xr_camera->get_quaternion();
                initial_camera_rotation = camera_rotation;
                // apply adjustment based on camera position/rotation to the initial origin transform
                xr_origin->set_position(initial_origin_position - camera_position);
                xr_position_initialized = true;
            }        
        }
        
        xr_node->set_global_rotation(Vector3(0, -Math_PI / 2, 0)); // set the rotation of the XR origin to match the camera

    }

    if (this->sub_view)
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        String velocity_text = "Linear Velocity: " + String::num(dragon_control->GetLinearVelocity(), 1) + "\n" + info_debug;
        label_info->set_text(velocity_text);
    }
}

Vector3 CameraControl::GetPostureHeadset()
{
    return xr_camera->get_global_rotation();
}