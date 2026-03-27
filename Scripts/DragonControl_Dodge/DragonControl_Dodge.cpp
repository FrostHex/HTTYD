#include "DragonControl_Dodge.h"

#include "DragonAnimator.h"
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


/**
 * @brief constructor
 */
DragonControl_Dodge::DragonControl_Dodge() 
{
    input_singleton = Input::get_singleton();
    species_gronckle = ResourceLoader::get_singleton()->load("res://Scenes/Dragons/GronckleRoot.tscn");
    set_physics_process(true);
}


/**
 * @brief destructor
 */
DragonControl_Dodge::~DragonControl_Dodge() 
{
}


/**
 * @brief bind methods to the Godot engine
 */
void DragonControl_Dodge::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("_deferred_attach_camera_to_socket_back"), &DragonControl_Dodge::_deferred_attach_camera_to_socket_back);
}


void DragonControl_Dodge::_deferred_attach_camera_to_socket_back()
{
    if (!camera_main || !dragon_rb)
    {
        return;
    }

    Node *species_slot = Object::cast_to<Node>(dragon_rb->get_node_or_null("SpeciesSlot"));
    Node *socket_back = nullptr;
    if (species_slot)
    {
        Node *dragon_root = Object::cast_to<Node>(species_slot->get_node_or_null("GronckleRoot"));
        if (dragon_root)
        {
            socket_back = Object::cast_to<Node>(dragon_root->get_node_or_null("Sockets/Socket_Back"));
        }
    }

    if (!socket_back)
    {
        if (camera_attach_retry_count < 30)
        {
            camera_attach_retry_count++;
            call_deferred("_deferred_attach_camera_to_socket_back");
        }
        else
        {
            UtilityFunctions::printerr("DragonControl_Dodge: Failed to attach camera, path 'SpeciesSlot/GronckleRoot/Sockets/Socket_Back' not found.");
        }
        return;
    }

    camera_main->reparent(socket_back);
    camera_main->set_position(Vector3(-0.2f, 0.8f, 0.0f));

    Node3D *camera_non_xr = Object::cast_to<Node3D>(camera_main->get_node_or_null("Camera_Main_NonXR"));
    if (camera_non_xr)
    {
        camera_non_xr->set_position(Vector3(0, 0, 0));
    }

    Vector3 euler = camera_main->get_basis().get_euler();
    cam_yaw = euler.y;
    cam_pitch = euler.z;
    camera_attach_retry_count = 0;

    DragonAnimator *dragon_animator = Object::cast_to<DragonAnimator>(get_parent()->get_node_or_null("DragonAnimator"));
    if (dragon_animator)
    {
        dragon_animator->call_deferred("RefreshBindings");
        dragon_animator->call_deferred("SetAnimation_Weight", "add_wing_main", 1.0f);
    }
    else
    {
        UtilityFunctions::printerr("DragonControl_Dodge: DragonAnimator not found.");
    }
}


/**
 * @brief get rigid body reference
 */
void DragonControl_Dodge::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) 
    {
        return;
    }

    // Capture mouse so movement is tracked even when the cursor would leave the window
    input_singleton->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);

    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (dragon_rb) 
    {
        dragon_rb->set_gravity_scale(0);
        Node *species_slot = Object::cast_to<Node>(dragon_rb->get_node_or_null("SpeciesSlot"));
        if (species_slot)
        {
            Node *toothless_root = Object::cast_to<Node>(species_slot->get_node_or_null("ToothlessRoot"));
            if (toothless_root)
            {
                toothless_root->queue_free();
            }

            if (species_gronckle.is_valid())
            {
                Node *gronckle_root = species_gronckle->instantiate();
                if (gronckle_root)
                {
                    gronckle_root->set_name("GronckleRoot");
                    // Defer add_child to avoid tree-setup timing issues in _ready.
                    species_slot->call_deferred("add_child", gronckle_root);
                }
                else
                {
                    UtilityFunctions::printerr("DragonControl_Dodge: Failed to instantiate Species_Gronckle scene.");
                }
            }
            else
            {
                UtilityFunctions::printerr("DragonControl_Dodge: Species_Gronckle is not assigned.");
            }
        }
        else
        {
            UtilityFunctions::printerr("DragonControl_Dodge: SpeciesSlot not found.");
        }

    }
    
    camera_main = Object::cast_to<Node3D>(get_parent()->get_parent()->get_parent()->get_node_or_null("Camera_Main"));
    if (camera_main)
    {
        camera_attach_retry_count = 0;
        call_deferred("_deferred_attach_camera_to_socket_back");
    }
    else
    {
        UtilityFunctions::printerr("DragonControl_Dodge: Camera_Main not found.");
    }
}


/**
 * @brief process input and set velocity every physics frame
 * @param delta time since last frame
 */
void DragonControl_Dodge::_physics_process(double delta) 
{
    if (!dragon_rb) 
    {
        return;
    }

    // Get current transform basis for local movement
    Basis basis = dragon_rb->get_global_transform().basis;

    // Linear velocity control
    Vector3 linear_velocity = Vector3(0, 0, 0);

    // Forward/Backward (W/S) - local forward direction
    float forward_input = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_S)) - 
                          static_cast<float>(input_singleton->is_key_pressed(Key::KEY_W));
    if (forward_input != 0.0f) 
    {
        linear_velocity -= basis.get_column(0) * forward_input * GRONCKLE_LINEAR_SPEED;
    }

    // Left/Right (A/D) - local right direction
    float strafe_input = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_D)) - 
                         static_cast<float>(input_singleton->is_key_pressed(Key::KEY_A));
    if (strafe_input != 0.0f) 
    {
        linear_velocity += basis.get_column(2) * strafe_input * GRONCKLE_LINEAR_SPEED;
    }

    // Up/Down (Space/Shift) - global up direction
    float vertical_input = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_SPACE)) - 
                           static_cast<float>(input_singleton->is_key_pressed(Key::KEY_SHIFT));
    if (vertical_input != 0.0f) 
    {
        linear_velocity += Vector3(0, 1, 0) * vertical_input * GRONCKLE_LINEAR_SPEED;
    }

    dragon_rb->set_linear_velocity(linear_velocity);

    // Angular velocity control
    Vector3 angular_velocity = Vector3(0, 0, 0);

    // Turn left/right (Q/E) - yaw rotation around global up axis
    float turn_input = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_Q)) - 
                       static_cast<float>(input_singleton->is_key_pressed(Key::KEY_E));
    if (turn_input != 0.0f) 
    {
        angular_velocity = Vector3(0, 1, 0) * turn_input * GRONCKLE_ANGULAR_SPEED;
    }

    dragon_rb->set_angular_velocity(angular_velocity);

    // Mouse look: only when mouse is captured
    if (camera_main && input_singleton->get_mouse_mode() == Input::MOUSE_MODE_CAPTURED)
    {
        Vector2 mouse_velocity = input_singleton->get_last_mouse_velocity();
        // Convert screen-space motion to radians; account for frame time
        float yaw_delta = -mouse_velocity.x * static_cast<float>(delta) * CAMERA_MOUSE_SENSITIVITY;
        float pitch_delta = -mouse_velocity.y * static_cast<float>(delta) * CAMERA_MOUSE_SENSITIVITY;

        cam_yaw += yaw_delta;
        cam_pitch += pitch_delta;

        // Clamp pitch to avoid flipping; adjust range as needed
        const float max_pitch = Math_PI * 0.49f;
        cam_pitch = Math::clamp(cam_pitch, -max_pitch, max_pitch);

        // Rebuild basis from yaw (Y) then pitch (Z) to eliminate roll drift
        Quaternion q_yaw(Vector3(0, 1, 0), cam_yaw);
        Quaternion q_pitch(Vector3(0, 0, 1), cam_pitch);
        Basis new_basis(q_yaw * q_pitch);
        camera_main->set_basis(new_basis);
    }
}


void DragonControl_Dodge::_input(const Ref<InputEvent> &event)
{
    if (!event.is_valid())
    {
        return;
    }

    // Release capture on Esc / Backspace / Enter key press
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

    // Toggle capture on right mouse button press; left button does nothing here
    const Ref<InputEventMouseButton> mb_event = event;
    if (mb_event.is_valid() && mb_event->is_pressed())
    {
        if (mb_event->get_button_index() == MouseButton::MOUSE_BUTTON_RIGHT)
        {
            Input::MouseMode mode = input_singleton->get_mouse_mode();
            if (mode == Input::MOUSE_MODE_CAPTURED)
            {
                input_singleton->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
            }
            else
            {
                input_singleton->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
            }
        }
    }
}
