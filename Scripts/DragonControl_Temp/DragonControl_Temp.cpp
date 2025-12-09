#include "DragonControl_Temp.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/quaternion.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

using namespace godot;


/**
 * @brief constructor
 */
DragonControl_Temp::DragonControl_Temp() 
{
    input_singleton = Input::get_singleton();
    set_physics_process(true);
}


/**
 * @brief destructor
 */
DragonControl_Temp::~DragonControl_Temp() 
{
}


/**
 * @brief bind methods to the Godot engine
 */
void DragonControl_Temp::_bind_methods() 
{
}


/**
 * @brief get rigid body reference
 */
void DragonControl_Temp::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) 
    {
        return;
    }

    // Capture mouse so movement is tracked even when the cursor would leave the window
    input_singleton->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);

    gronckle_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (gronckle_rb) 
    {
        gronckle_rb->set_gravity_scale(0);
    }
    camera_main = get_parent()->get_node<Node3D>("Camera_Main/Camera_Main_NonXR");

    // Initialize yaw/pitch to current camera orientation if available
    if (camera_main)
    {
        Vector3 euler = camera_main->get_basis().get_euler();
        cam_yaw = euler.y;
        cam_pitch = euler.z; // using z as the right axis (x forward, z right, y up)
    }
}


/**
 * @brief process input and set velocity every physics frame
 * @param delta time since last frame
 */
void DragonControl_Temp::_physics_process(double delta) 
{
    if (!gronckle_rb) 
    {
        return;
    }

    // Get current transform basis for local movement
    Basis basis = gronckle_rb->get_global_transform().basis;

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

    gronckle_rb->set_linear_velocity(linear_velocity);

    // Angular velocity control
    Vector3 angular_velocity = Vector3(0, 0, 0);

    // Turn left/right (Q/E) - yaw rotation around global up axis
    float turn_input = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_Q)) - 
                       static_cast<float>(input_singleton->is_key_pressed(Key::KEY_E));
    if (turn_input != 0.0f) 
    {
        angular_velocity = Vector3(0, 1, 0) * turn_input * GRONCKLE_ANGULAR_SPEED;
    }

    gronckle_rb->set_angular_velocity(angular_velocity);

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
        Quaternion q_pitch(Vector3(1, 0, 0), cam_pitch);
        Basis new_basis(q_yaw * q_pitch);
        camera_main->set_basis(new_basis);
    }
}


void DragonControl_Temp::_input(const Ref<InputEvent> &event)
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
