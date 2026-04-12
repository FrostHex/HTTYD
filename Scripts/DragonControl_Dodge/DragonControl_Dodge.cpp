#include "DragonControl_Dodge.h"

#include "DragonAnimator.h"
#include "Control_Main.h"
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
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
    ClassDB::bind_method(D_METHOD("_on_back_button_pressed"), &DragonControl_Dodge::_on_back_button_pressed);
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
                    // species_slot->call_deferred("add_child", gronckle_root);
                    species_slot->add_child(gronckle_root);
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

    Node *scene_root = get_parent() ? get_parent()->get_parent() : nullptr;
    if (scene_root)
    {
        Node *back_node = scene_root->get_node_or_null(NodePath("UI/Button_Back"));
        if (Button *back_button = Object::cast_to<Button>(back_node))
        {
            back_button->connect("pressed", callable_mp(this, &DragonControl_Dodge::_on_back_button_pressed));
        }
    }
    
    camera_main = Object::cast_to<Node3D>(get_parent()->get_parent()->get_parent()->get_node_or_null("Camera_Main"));
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

void DragonControl_Dodge::_on_back_button_pressed()
{
    UtilityFunctions::print("Back button pressed, returning to Scene_Home");

    SceneTree *tree = get_tree();
    if (!tree)
    {
        UtilityFunctions::printerr("SceneTree not available");
        return;
    }

    Window *root = tree->get_root();
    if (!root)
    {
        UtilityFunctions::printerr("Root window not available");
        return;
    }

    set_physics_process(false);
    set_process_input(false);
    if (dragon_rb && dragon_rb->is_inside_tree())
    {
        dragon_rb->set_linear_velocity(Vector3(0.0f, 0.0f, 0.0f));
        dragon_rb->set_angular_velocity(Vector3(0.0f, 0.0f, 0.0f));
    }
    if (input_singleton)
    {
        input_singleton->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
    }

    if (camera_main && camera_main->is_inside_tree())
    {
        Node *main_node = root->get_node_or_null(NodePath("Main"));
        if (main_node)
        {
            camera_main->reparent(main_node);
            camera_main->call_deferred("set_transform", Transform3D(Basis(), Vector3(0.0f, 10.0f, 0.0f)));

            Node *xr_origin = camera_main->get_node_or_null(NodePath("XR/XROrigin"));
            if (xr_origin)
            {
                xr_origin->call_deferred("set_position", Vector3(0.0f, 0.0f, 0.0f));
                Node *sub_viewport_mesh = xr_origin->get_node_or_null(NodePath("XRCamera/SubViewportMesh"));
                if (sub_viewport_mesh)
                {
                    sub_viewport_mesh->queue_free();
                }
            }
        }
    }

    Control_Main *control_main = Object::cast_to<Control_Main>(root->get_node_or_null(NodePath("Main/Control_Main")));
    if (!control_main)
    {
        Node *cm = root->find_child("Control_Main", true, false);
        control_main = Object::cast_to<Control_Main>(cm);
    }

    if (control_main)
    {
        control_main->call("Switch_Scene", "Scene_Home");
    }
    else
    {
        UtilityFunctions::printerr("DragonControl_Dodge: Control_Main not available to switch scene.");
    }
}
