#include "Control_Camera.h"
#include "Control_Main.h"

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
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>

using namespace godot;


/**
 * @brief constructor
 */
Control_Camera::Control_Camera() 
{
}


/**
 * @brief destructor
 */
Control_Camera::~Control_Camera() 
{
}

void Control_Camera::SetDragonControl(DragonControlTop* dragon_control) 
{
    if (dragon_control)
    {
        this->dragon_control = dragon_control;
        // connect the "dragon_collision" signal emitted by DragonControlTop
        this->dragon_control->connect("dragon_collision", Callable(this, "Print_Collision"));
    }
}


/**
 * @brief initialize the sub camera if the "Sub View" property is enabled
 */
void Control_Camera::_ready()
{
    // Get reference to Control_Main
    SceneTree *tree = get_tree();
    if (tree) 
    {
        Window *root = tree->get_root();
        if (root) 
        {
            control_main = Object::cast_to<Control_Main>(root->get_node_or_null(NodePath("Main/Control_Main")));
            if (!control_main) 
            {
                UtilityFunctions::printerr("Control_Camera: Could not find Control_Main at Main/Control_Main");
                return;
            }
        }
    }

    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    xr_node = get_parent()->get_node<Node>("Camera_Main")->get_node<Node3D>("XR");
    xr_origin = xr_node->get_node<Node3D>("XROrigin");
    xr_camera = xr_origin->get_node<Node3D>("XRCamera");
    camera_main = dragon_rb->get_node<Node3D>("Camera_Main");

    if (!Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        set_physics_process(false);

        if (control_main->GetValEnableHeadset())
        {
            initial_origin_position = xr_origin->get_position();
            xr_position_initialized = false;
            set_physics_process(true);
        }

        if (control_main->GetValSubView())
        {
            Node* sub_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node* sub_viewport = sub_container->get_node<Node>("SubViewport");
            camera_sub = sub_viewport->get_node<Camera3D>("Camera_Sub");
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0));

            if (control_main->GetValDebug())
            {
                label_info = sub_viewport->get_node<Label>("Info");
            }

            if (control_main->GetValEnableHeadset())
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
                sub_mesh->set_position(Vector3(0.27f, 0.24f, -0.5f));
                sub_mesh->set_rotation(Vector3(Math_PI / 2, 0, 0));
                sub_mesh->set_name("SubViewportMesh");
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
void Control_Camera::_physics_process(double delta) 
{
    if (control_main->GetValEnableHeadset()) 
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

        if (camera_stabilized)
        {
            xr_node->set_global_rotation(Vector3(0, -Math_PI, 0)); // set the rotation of the XR origin to match the camera
        }
    }

    if (control_main->GetValSubView())
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        if (control_main->GetValDebug() && label_info)
        {
            String velocity_text = "Linear Velocity: " + String::num(dragon_control->GetLinearVelocity(), 1) + "\n" + info_debug + "\n" + time_elapsed;
            label_info->set_text(velocity_text);
        }
    }

    if (camera_offset_factor != 0.0f)
    {
        // UtilityFunctions::print("Camera offset factor: " + String::num(camera_offset_factor));
        // camera_main->set_position(dragon_rb->get_global_position());
        camera_main->set_position(camera_main->get_position() + Vector3(0, dragon_control->GetLinearVelocity() * camera_offset_factor * delta, 0));
        
    }

    if (approaching_angle)
    {   
        Vector3 current_rotation = camera_main->get_rotation();
        Vector3 delta_rotation = target_rotation - current_rotation;

        if (p_gain < 0.0f) 
        {
            delta_rotation = Vector3(0, 0, 10); 
        }
        else
        {
            p_gain += delta * (p_gain + 0.1f);
            if (p_gain > 5.0f) 
            {
                p_gain = 5.0f;
            }
        }

        // Wrap angles to [-PI, PI] for smooth interpolation
        for (int i = 0; i < 3; ++i) 
        {
            while (delta_rotation[i] > Math_PI) delta_rotation[i] -= 2 * Math_PI;
            while (delta_rotation[i] < -Math_PI) delta_rotation[i] += 2 * Math_PI;
        }

        Vector3 new_rotation = current_rotation + delta_rotation * p_gain * delta;
        camera_main->set_rotation(new_rotation);

        // Stop approaching if close enough
        if (delta_rotation.length() < 0.01f) 
        {
            camera_main->set_rotation(target_rotation);
            approaching_angle = false;
            p_gain = 0.0f;
        }
    }

    if (approaching_position)
    {
        Vector3 current_position = camera_main->get_position();
        Vector3 delta_position = dragon_rb->get_position() + target_position_offset - current_position;

        camera_main->set_position(current_position + delta_position * delta);

        // Stop approaching if close enough
        // if (delta_position.x + delta_position.y + delta_position.z < 0.001f) 
        // {
        //     camera_main->set_position(dragon_rb->get_position() + target_position_offset);
        //     approaching_position = false;
        // }
    }
}


Vector3 Control_Camera::GetPostureHeadset()
{
    return xr_camera->get_global_rotation();
}


void Control_Camera::Print_Collision(Node* body, float velocity)
{
    if (control_main->GetValDebug() && label_info)
    {
        String collision_info = "Collision with " + body->get_name() + " at velocity: " + String::num(velocity, 1);
        info_debug = collision_info;
    }
}

/**
 * @brief called when an input event occurs
 * @param event the input event
 */
void Control_Camera::_input(const Ref<InputEvent> &event) 
{
    if (event->is_action_pressed("save_state")) 
    {
        info_debug = "State Saved";
    }
    if (event->is_action_pressed("load_state")) 
    {
        info_debug = "State Loaded";
        if (control_main->GetValDebug() && label_info)
        {
            label_info->set_modulate(Color(0.863f, 0.953f, 1.0f, 1.0f));
        }
        if (camera_main && camera_main->get_parent() && camera_main->get_parent()->get_name() == String("Main")) 
        {
            camera_main->reparent(dragon_rb);
            camera_main->set_rotation(Vector3(0, 0, 0));
            camera_main->set_position(Vector3(0, 0, 0));
        }
        camera_offset_factor = 0.0f; // reset camera offset factor
        approaching_angle = false; // reset approaching angle flag
        approaching_position = false; // reset approaching position flag
    }
}


void Control_Camera::SetCameraOffsetFactor(float factor) 
{
    if (camera_offset_factor == 0.0f)
    {
        camera_main->reparent(dragon_rb->get_parent());
    }   
    camera_offset_factor = factor;
}


void Control_Camera::TriggerApproachingAngle(Vector3 target_rotation, float p_gain) 
{
    this->target_rotation = target_rotation;
    approaching_angle = true;
    this->p_gain = p_gain;
}


void Control_Camera::TriggerApproachingPosition(Vector3 target_position_offset)
{
    this->target_position_offset = target_position_offset;
    approaching_position = true;
}


void Control_Camera::GrabSaddle()
{
    approaching_angle = false;
    approaching_position = false;
    camera_offset_factor = 0.0f; // reset camera offset factor
    camera_main->reparent(dragon_rb);
    camera_main->call_deferred("set_position", Vector3(0, 0, 0));
    camera_main->call_deferred("set_rotation", Vector3(0, 0, 0));
}


void Control_Camera::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("Print_Collision", "body", "velocity"), &Control_Camera::Print_Collision);
    ClassDB::bind_method(D_METHOD("SetCameraOffsetFactor", "factor"), &Control_Camera::SetCameraOffsetFactor);
    ClassDB::bind_method(D_METHOD("TriggerApproachingAngle", "target_rotation", "p_gain"), &Control_Camera::TriggerApproachingAngle);
    ClassDB::bind_method(D_METHOD("TriggerApproachingPosition", "target_position_offset"), &Control_Camera::TriggerApproachingPosition);
    ClassDB::bind_method(D_METHOD("GrabSaddle"), &Control_Camera::GrabSaddle);
    ClassDB::bind_method(D_METHOD("SetCameraStabilized", "stabilized"), &Control_Camera::SetCameraStabilized);
}
