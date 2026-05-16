#include "Control_Camera.h"
#include "Control_Main.h"
#include "Settings.h"

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
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>

using namespace godot;


/**
 * @brief constructor
 */
Control_Camera::Control_Camera() 
{
    input_singleton = Input::get_singleton();
}


/**
 * @brief destructor
 */
Control_Camera::~Control_Camera() 
{
}

void Control_Camera::SetDragon_Pilot_(Dragon_Pilot_Top* dragon_pilot) 
{
    if (dragon_pilot)
    {
        this->dragon_pilot = dragon_pilot;
        // connect the "dragon_collision" signal emitted by Dragon_Pilot_Top
        this->dragon_pilot->connect("dragon_collision", Callable(this, "Print_Collision"));
    }
}


void Control_Camera::_ready()
{
    set_physics_process_priority(-80); // run before XRToolsHand (-70) to make sure the hand mesh position is correct
    set_process_input(false);
    set_physics_process(false);
}

/**
 * @brief initialize the sub camera if the "Sub View" property is enabled
 */
void Control_Camera::Initialize()
{
    set_process_input(true);
    // Find the node starting with "Scene_" and not "Scene_Home"
    Node* scene_node = nullptr;
    Node* main_node = get_tree()->get_root()->get_node_or_null(NodePath("Main"));
    if (main_node)
    {
        for (int i = 0; i < main_node->get_child_count(); ++i)
        {
            Node* child = main_node->get_child(i);
            if (child && String(child->get_name()).begins_with("Scene_") && child->get_name() != String("Scene_Home"))
            {
                scene_node = child;
                break;
            }
        }
    }
    if (!scene_node)
    {
        UtilityFunctions::printerr("Control_Camera: Could not find Scene_ node");
        return;
    }

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

            dragon_rb = scene_node->get_node<RigidBody3D>("Dragon");
            Node* species_slot = dragon_rb->get_node<Node>("SpeciesSlot");
            Node *latest_species = species_slot->get_child(species_slot->get_child_count() - 1);
            camera_main = latest_species->get_node<Node3D>("Sockets/Socket_Back_Mount/Socket_Back/Camera_Main");    
            xr_node = camera_main->get_node<Node3D>("XR");
            xr_origin = xr_node->get_node<Node3D>("XROrigin");
            xr_camera = xr_origin->get_node<Node3D>("XRCamera");
        }
    }

    if (!Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        set_physics_process(false);

        if (Settings::GetSingleton()->GetValEnableHeadset())
        {
            set_physics_process(true);
        }

        if (Settings::GetSingleton()->GetValSubView())
        {
            Node* sub_container = scene_node->get_node<Node>("Dragon/SubViewportContainer");
            Node* sub_viewport = sub_container->get_node<Node>("SubViewport");
            camera_sub = sub_viewport->get_node<Camera3D>("Camera_Sub");
            camera_sub->set_rotation(Vector3(0, - Math_PI / 2, 0));
            set_physics_process(true);

            if (Settings::GetSingleton()->GetValDebugInfo())
            {
                label_info = sub_viewport->get_node<Label>("Info");
            }

            if (Settings::GetSingleton()->GetValEnableHeadset())
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
        } 
        else // destroy the SubViewportContainer and all its children 
        {
            if (Node *container = get_parent()->get_node<Node>("SubViewportContainer")) 
            {
                container->queue_free();
            }
            return;
        }
    }
}


void Control_Camera::ResetVRTransform()
{
    if (get_parent()->get_parent()->has_node(NodePath("Camera_Main")))
    {
        if (!xr_camera)
        {
            xr_origin = get_parent()->get_parent()->get_node<Node3D>("Camera_Main/XR/XROrigin");
            xr_camera = xr_origin->get_node<Node3D>("XRCamera");
        }

        if (vr_recenter_pending)
        {
            // Phase 2: run on next frame after center_on_hmd updates camera pose.
            vr_recenter_pending = false;
            xr_origin->set_position(xr_origin->get_position() - xr_camera->get_position()); // compensate for the height of the headset
            return;
        }

        Vector3 camera_position = xr_camera->get_position();
        if (camera_position.length_squared() > 0.001f) // check if camera position is not zero vector
        {
            XRServer *xr_server = XRServer::get_singleton();
            if (!xr_server)
            {
                return;
            }

            Ref<XRInterface> xr_interface = xr_server->get_primary_interface();
            if (xr_interface.is_null() || !xr_interface->is_initialized())
            {
                UtilityFunctions::printerr("Control_Camera: primary XR interface is not available/initialized");
                return;
            }

            // recenter through XRServer so XROrigin/XRCamera are updated in Godot space.
            xr_server->center_on_hmd(XRServer::RESET_FULL_ROTATION, true);
            vr_recenter_pending = true;
            Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.0);
            timer->connect("timeout", Callable(this, "ResetVRTransform"));
        }
        else
        {
            Ref<SceneTreeTimer> timer = get_tree()->create_timer(0.1);
            timer->connect("timeout", Callable(this, "ResetVRTransform"));
        }
    }
}


/**
 * @brief let the sub camera follow the dragon
 */
void Control_Camera::_physics_process(double delta)
{
    if (Settings::GetSingleton()->GetValEnableHeadset() && camera_stabilized)
    {
        xr_node->set_global_rotation(Vector3(0, -Math_PI, 0)); // set the rotation of the XR origin to match the camera
    }
    // return;
    if (Settings::GetSingleton()->GetValSubView())
    {
        camera_sub->set_global_position(Vector3(-8.729f, 1.797f, 0) + dragon_rb->get_global_transform().origin);
        if (Settings::GetSingleton()->GetValDebugInfo() && dragon_pilot && label_info)
        {
            String velocity_text = "Linear Velocity: " + String::num(dragon_pilot->GetLinearVelocity(), 1) + "\n" + info_debug + "\n" + time_elapsed;
            label_info->set_text(velocity_text);
        }
    }

    if (dragon_rb && Math::abs(camera_offset_factor) > 0.01f)
    {
        // UtilityFunctions::print("Camera offset factor: " + String::num(camera_offset_factor));
        // camera_main->set_position(dragon_rb->get_global_position());
        camera_main->set_position(camera_main->get_position() + Vector3(0, dragon_pilot->GetLinearVelocity() * camera_offset_factor * delta, 0));
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

    if (resetting_transform)
    {
        timer -= delta;
        // UtilityFunctions::print("Resetting camera transform, time left: " + String::num(timer));
        if (timer > 0.0f)
        {
            float ratio = timer / resetting_transform_time;
            camera_main->set_position(target_position_offset * ratio);
            camera_main->set_rotation(target_rotation * ratio);
        }
        else
        {
            camera_main->set_position(Vector3(0, 0, 0));
            camera_main->set_rotation(Vector3(0, 0, 0));
            resetting_transform = false;
        }
    }

    // Free camera: mouse-look when free_camera is enabled and mouse is captured
    if (free_camera && input_singleton && input_singleton->get_mouse_mode() == Input::MOUSE_MODE_CAPTURED)
    {
        Vector2 mouse_velocity = input_singleton->get_last_mouse_velocity();
        float yaw_delta   = -mouse_velocity.x * static_cast<float>(delta) * CAMERA_MOUSE_SENSITIVITY;
        float pitch_delta = -mouse_velocity.y * static_cast<float>(delta) * CAMERA_MOUSE_SENSITIVITY;

        cam_yaw   += yaw_delta;
        cam_pitch += pitch_delta;

        const float max_pitch = Math_PI * 0.49f;
        cam_pitch = Math::clamp(cam_pitch, -max_pitch, max_pitch);

        Quaternion q_yaw(Vector3(0, 1, 0), cam_yaw);
        Quaternion q_pitch(Vector3(0, 0, 1), cam_pitch);
        camera_main->set_basis(Basis(q_yaw * q_pitch));
    }
}


Vector3 Control_Camera::GetPostureHeadset()
{
    if (!xr_camera || !xr_camera->is_inside_tree())
    {
        return Vector3();
    }
    return xr_camera->get_global_rotation();
}


void Control_Camera::Print_Collision(Node* body, float velocity)
{
    if (control_main && Settings::GetSingleton()->GetValDebugInfo() && label_info)
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
    // Skip in editor mode or if not initialized
    if (Engine::get_singleton()->is_editor_hint() || !control_main)
    {
        return;
    }

    if (event->is_action_pressed("save_state")) 
    {
        info_debug = "State Saved";
    }
    if (event->is_action_pressed("load_state")) 
    {
        info_debug = "State Loaded";
        if (Settings::GetSingleton()->GetValDebugInfo() && label_info)
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

    // Free camera: release mouse capture on Esc/Backspace/Enter; toggle on RMB
    if (free_camera && input_singleton)
    {
        const Ref<InputEventKey> key_event = event;
        if (key_event.is_valid() && key_event->is_pressed())
        {
            int32_t keycode = key_event->get_keycode();
            if (keycode == Key::KEY_ESCAPE || keycode == Key::KEY_BACKSPACE || keycode == Key::KEY_ENTER)
            {
                input_singleton->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
                return;
            }
        }

        const Ref<InputEventMouseButton> mb_event = event;
        if (mb_event.is_valid() && mb_event->is_pressed())
        {
            if (mb_event->get_button_index() == MouseButton::MOUSE_BUTTON_RIGHT)
            {
                Input::MouseMode mode = input_singleton->get_mouse_mode();
                input_singleton->set_mouse_mode(
                    mode == Input::MOUSE_MODE_CAPTURED
                        ? Input::MOUSE_MODE_VISIBLE
                        : Input::MOUSE_MODE_CAPTURED);
            }
        }
    }
}


void Control_Camera::SetCameraOffsetFactor(float factor)
{
    if (!camera_main || !dragon_rb)
    {
        return;
    }
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
    if (!camera_main || !dragon_rb)
    {
        return;
    }
    approaching_angle = false;
    approaching_position = false;
    camera_offset_factor = 0.0f; // reset camera offset factor
    camera_main->reparent(dragon_rb->get_node<Node>("SpeciesSlot/ToothlessRoot/Sockets/Socket_Back_Mount/Socket_Back"));
    resetting_transform_time = timer = 1.5f;
    resetting_transform = true;
    target_position_offset = camera_main->get_position();
    target_rotation = camera_main->get_rotation();
    UtilityFunctions::print(target_position_offset);
    set_physics_process(true);
    // camera_main->call_deferred("set_position", Vector3(0, 0, 0));
    // camera_main->call_deferred("set_rotation", Vector3(0, 0, 0));
}


void Control_Camera::ReparentCamera(const NodePath &target_path)
{
    if (!camera_main || !dragon_rb)
    {
        UtilityFunctions::printerr("Control_Camera: camera_main or dragon_rb not ready for reparent.");
        return;
    }
    Node *target = dragon_rb->get_node_or_null(target_path);
    if (!target)
    {
        UtilityFunctions::printerr("Control_Camera: reparent target not found: " + String(target_path));
        return;
    }
    camera_main->reparent(target);
}


void Control_Camera::SetFreeCamera(bool enabled)
{
    if (enabled && Settings::GetSingleton()->GetValEnableHeadset())
    {
        return; // headset mode is active, free camera cannot be enabled
    }

    free_camera = enabled;

    if (free_camera)
    {
        if (input_singleton)
        {
            input_singleton->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
        }
    }
    else
    {
        if (input_singleton)
        {
            input_singleton->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
        }
    }
}


void Control_Camera::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("Print_Collision", "body", "velocity"), &Control_Camera::Print_Collision);
    ClassDB::bind_method(D_METHOD("SetCameraOffsetFactor", "factor"), &Control_Camera::SetCameraOffsetFactor);
    ClassDB::bind_method(D_METHOD("TriggerApproachingAngle", "target_rotation", "p_gain"), &Control_Camera::TriggerApproachingAngle);
    ClassDB::bind_method(D_METHOD("TriggerApproachingPosition", "target_position_offset"), &Control_Camera::TriggerApproachingPosition);
    ClassDB::bind_method(D_METHOD("GrabSaddle"), &Control_Camera::GrabSaddle);
    ClassDB::bind_method(D_METHOD("SetCameraStabilized", "stabilized"), &Control_Camera::SetCameraStabilized);
    ClassDB::bind_method(D_METHOD("Initialize"), &Control_Camera::Initialize);
    ClassDB::bind_method(D_METHOD("ResetVRTransform"), &Control_Camera::ResetVRTransform);
    ClassDB::bind_method(D_METHOD("SetFreeCamera", "enabled"), &Control_Camera::SetFreeCamera);
    ClassDB::bind_method(D_METHOD("ReparentCamera", "target_path"), &Control_Camera::ReparentCamera);
}