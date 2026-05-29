#include "Dragon_Pilot_Dodge.h"

#include "Dragon_Animator.h"
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
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


/**
 * @brief constructor
 */
Dragon_Pilot_Dodge::Dragon_Pilot_Dodge() 
{
    input_singleton = Input::get_singleton();
    species_gronckle = ResourceLoader::get_singleton()->load("res://Tscn/Dragons/GronckleRoot.tscn");
    set_physics_process(true);
    // Dragon_Animator* animator = get_parent()->get_node<Dragon_Animator>("Dragon_Animator");
    // if (animator) 
    // {
    //     // animator->RefreshBindings();
    //     UtilityFunctions::print("D!!!!!!!!!!!!!!!");
    // }
}


/**
 * @brief destructor
 */
Dragon_Pilot_Dodge::~Dragon_Pilot_Dodge() 
{
}


/**
 * @brief bind methods to the Godot engine
 */
void Dragon_Pilot_Dodge::_bind_methods() 
{
    // ClassDB::bind_method(D_METHOD("ReturnHome"), &Dragon_Pilot_Dodge::ReturnHome);
}


/**
 * @brief get rigid body reference
 */
void Dragon_Pilot_Dodge::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) 
    {
        return;
    }

    // Capture mouse so movement is tracked even when the cursor would leave the window
    // (Handled by Control_Camera when free_camera is enabled)

    dragon_rb = Object::cast_to<RigidBody3D>(get_parent());
    if (dragon_rb) 
    {
        dragon_rb->set_gravity_scale(0);
        Node *species_slot = Object::cast_to<Node>(dragon_rb->get_node_or_null("SpeciesSlot"));
        if (species_slot)
        {
            Node *toothless_root = Object::cast_to<Node>(species_slot->get_node_or_null("ToothlessRoot"));
            Node *gronckle_root = nullptr;

            if (species_gronckle.is_valid())
            {
                gronckle_root = species_gronckle->instantiate();
                if (gronckle_root)
                {
                    gronckle_root->set_name("GronckleRoot");
                    // Defer add_child to avoid tree-setup timing issues in _ready.
                    // species_slot->call_deferred("add_child", gronckle_root);
                    species_slot->add_child(gronckle_root);
                }
                else
                {
                    UtilityFunctions::printerr("Dragon_Pilot_Dodge: Failed to instantiate Species_Gronckle scene.");
                }
            }
            else
            {
                UtilityFunctions::printerr("Dragon_Pilot_Dodge: Species_Gronckle is not assigned.");
            }

            if (toothless_root)
            {
                // Avoid freeing Camera_Main if it was attached to Toothless before Gronckle is spawned.
                Node3D *camera_node = Object::cast_to<Node3D>(
                    toothless_root->find_child("Camera_Main", true, false));
                if (camera_node)
                {
                    Node *gronckle_socket = gronckle_root
                        ? gronckle_root->get_node_or_null("Sockets/Socket_Back_Mount/Socket_Back")
                        : nullptr;
                    if (gronckle_socket)
                    {
                        camera_node->reparent(gronckle_socket);
                        camera_node->set_position(Vector3(0, 0, 0));
                        camera_node->set_rotation(Vector3(0, 0, 0));
                    }
                    else
                    {
                        Node *main_node = get_tree()
                            ? get_tree()->get_root()->get_node_or_null("Main")
                            : nullptr;
                        if (main_node)
                            camera_node->reparent(main_node);
                    }
                }
                toothless_root->queue_free();
            }
        }
        else
        {
            UtilityFunctions::printerr("Dragon_Pilot_Dodge: SpeciesSlot not found.");
        }
    }

    // Node *scene_root = get_parent() ? get_parent()->get_parent() : nullptr;
    // if (scene_root)
    // {
    //     Node *back_node = scene_root->get_node_or_null(NodePath("UI/Button_Back"));
    //     if (Button *back_button = Object::cast_to<Button>(back_node))
    //     {
    //         back_button->connect("pressed", callable_mp(this, &Dragon_Pilot_Dodge::ReturnHome));
    //     }
    // }
}

/**
 * @brief process input and set velocity every physics frame
 * @param delta time since last frame
 */
void Dragon_Pilot_Dodge::_physics_process(double delta) 
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
}


void Dragon_Pilot_Dodge::_input(const Ref<InputEvent> &event)
{
    // Mouse capture and free-camera input is now handled by Control_Camera::_input.
    if (!event.is_valid())
    {
        return;
    }
}