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
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>

using namespace godot;


/**
 * @brief constructor
 * @param sub_view whether to use the sub camera
 * @param debug whether to enable debug mode
 * @param enable_headset whether to enable the XR headset
 */
CameraControl::CameraControl(bool sub_view, bool debug, bool enable_headset) 
{
    this->sub_view = sub_view;
    this->debug = debug;
    this->enable_headset = enable_headset;

    if (enable_headset) 
    {
        DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
        Ref<XRInterface> xr_interface = XRServer::get_singleton()->find_interface("OpenXR");
        if (xr_interface.is_valid()) 
        {
            // if (xr_interface->has_method("set_render_target_size_multiplier")) 
            // {
            //     xr_interface->call("set_render_target_size_multiplier", 0.5f); // decrease resolution
            // }
            // if (xr_interface->has_method("set_display_refresh_rate")) 
            // {
            //     xr_interface->call("set_display_refresh_rate", 30.0f);
            // }
        }
        Engine* engine = Engine::get_singleton();
        if (engine) 
        {
            engine->set_physics_ticks_per_second(60);
            // engine->set_max_fps(30);
        }
    }
}


/**
 * @brief destructor
 */
CameraControl::~CameraControl() 
{
}

void CameraControl::SetDragonControl(DragonControlTop* dragon_control) 
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
void CameraControl::_ready()
{
    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    xr_node = get_parent()->get_node<Node>("Camera_Main")->get_node<Node3D>("XR");
    xr_origin = xr_node->get_node<Node3D>("XROrigin");
    xr_camera = xr_origin->get_node<Node3D>("XRCamera");
    camera_main = dragon_rb->get_node<Node3D>("Camera_Main");

    if (!Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        set_physics_process(false);

        if (this->enable_headset)
        {
            initial_origin_position = xr_origin->get_position();
            xr_position_initialized = false;
            set_physics_process(true);
            DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
        }
        else
        {
            xr_node->queue_free();
        }

        if (this->sub_view)
        {
            Node* sub_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node* sub_viewport = sub_container->get_node<Node>("SubViewport");
            camera_sub = sub_viewport->get_node<Camera3D>("Camera_Sub");
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0));

            if (this->debug)
            {
                label_info = sub_viewport->get_node<Label>("Info");
            }

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

        if (camera_stabilized)
        {
            xr_node->set_global_rotation(Vector3(0, -Math_PI, 0)); // set the rotation of the XR origin to match the camera
        }
    }

    if (this->sub_view)
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        if (this->debug && label_info)
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


Vector3 CameraControl::GetPostureHeadset()
{
    return xr_camera->get_global_rotation();
}


void CameraControl::Print_Collision(Node* body, float velocity)
{
    if (debug && label_info)
    {
        String collision_info = "Collision with " + body->get_name() + " at velocity: " + String::num(velocity, 1);
        info_debug = collision_info;
    }
}

/**
 * @brief called when an input event occurs
 * @param event the input event
 */
void CameraControl::_input(const Ref<InputEvent> &event) 
{
    if (event->is_action_pressed("save_state")) 
    {
        info_debug = "State Saved";
    }
    if (event->is_action_pressed("load_state")) 
    {
        info_debug = "State Loaded";
        if (debug && label_info)
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


void CameraControl::SetCameraOffsetFactor(float factor) 
{
    if (camera_offset_factor == 0.0f)
    {
        camera_main->reparent(dragon_rb->get_parent());
    }   
    camera_offset_factor = factor;
}


void CameraControl::TriggerApproachingAngle(Vector3 target_rotation, float p_gain) 
{
    this->target_rotation = target_rotation;
    approaching_angle = true;
    this->p_gain = p_gain;
}


void CameraControl::TriggerApproachingPosition(Vector3 target_position_offset)
{
    this->target_position_offset = target_position_offset;
    approaching_position = true;
}


void CameraControl::GrabSaddle()
{
    approaching_angle = false;
    approaching_position = false;
    camera_offset_factor = 0.0f; // reset camera offset factor
    camera_main->reparent(dragon_rb);
    camera_main->call_deferred("set_position", Vector3(0, 0, 0));
    camera_main->call_deferred("set_rotation", Vector3(0, 0, 0));
}


void CameraControl::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("Print_Collision", "body", "velocity"), &CameraControl::Print_Collision);
    ClassDB::bind_method(D_METHOD("SetCameraOffsetFactor", "factor"), &CameraControl::SetCameraOffsetFactor);
    ClassDB::bind_method(D_METHOD("TriggerApproachingAngle", "target_rotation", "p_gain"), &CameraControl::TriggerApproachingAngle);
    ClassDB::bind_method(D_METHOD("TriggerApproachingPosition", "target_position_offset"), &CameraControl::TriggerApproachingPosition);
    ClassDB::bind_method(D_METHOD("GrabSaddle"), &CameraControl::GrabSaddle);
    ClassDB::bind_method(D_METHOD("SetCameraStabilized", "stabilized"), &CameraControl::SetCameraStabilized);
}
