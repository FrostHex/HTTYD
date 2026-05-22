// ==================== Seagulls.cpp ====================
#include "Seagulls.h"

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>

using namespace godot;

Seagulls::Seagulls()
{
}
Seagulls::~Seagulls() {}

void Seagulls::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    initialize_seagulls();
}

void Seagulls::initialize_seagulls()
{
    Path3D *path = Object::cast_to<Path3D>(get_node_or_null("Path3D"));
    ERR_FAIL_NULL_MSG(path, "Seagulls requires a child Path3D node for orientation.");

    Ref<Curve3D> curve = path->get_curve();
    ERR_FAIL_COND_MSG(curve.is_null() || curve->get_point_count() < 2,
        "Seagulls requires Path3D to have a Curve3D with at least 2 points.");

    Vector3 forward = curve->get_point_position(1) - curve->get_point_position(0);
    float initial_yaw = 0.0f;
    Vector3 move_velocity = Vector3();
    if (forward.length_squared() > 0.0001f)
    {
        Vector3 local_direction = path->get_transform().basis.xform(forward);
        if (local_direction.length_squared() > 0.0001f)
        {
            local_direction = local_direction.normalized();
            initial_yaw = Math::atan2(-local_direction.x, -local_direction.z) - Math_PI * 0.5f;
        }

        Vector3 global_direction = path->get_global_transform().basis.xform(forward);
        if (global_direction.length_squared() > 0.0001f)
        {
            move_velocity = global_direction.normalized() * speed;
        }
    }

    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();

    int child_count = get_child_count();
    for (int i = 0; i < child_count; i++)
    {
        Node *child = get_child(i);
        String child_name = String(child->get_name());
        if (!child_name.begins_with("Seagull"))
        {
            continue;
        }

        Node3D *seagull = Object::cast_to<Node3D>(child);
        if (!seagull)
        {
            continue;
        }

        Vector3 rotation = seagull->get_rotation();
        rotation.y = initial_yaw;
        seagull->set_rotation(rotation);

        if (move_velocity.length_squared() > 0.0001f)
        {
            RigidBody3D *body = Object::cast_to<RigidBody3D>(seagull);
            if (body)
            {
                body->set_linear_velocity(move_velocity);
            }
        }

        Node *anim_root = nullptr;
        if (seagull->get_child_count() > 0)
        {
            anim_root = seagull->get_child(0);
        }

        Node *anim_node = nullptr;
        if (anim_root)
        {
            anim_node = anim_root->get_node_or_null("AnimationPlayer");
        }
        if (!anim_node)
        {
            anim_node = seagull->get_node_or_null("AnimationPlayer");
        }
        AnimationPlayer *anim = Object::cast_to<AnimationPlayer>(anim_node);
        if (anim)
        {
            StringName anim_name = anim->get_current_animation();
            if (anim_name.is_empty())
            {
                PackedStringArray names = anim->get_animation_list();
                if (names.size() > 0)
                {
                    anim_name = names[0];
                }
            }

            if (!anim_name.is_empty())
            {
                anim->play(anim_name);
                double length = anim->get_current_animation_length();
                if (length > 0.0)
                {
                    double offset = rng->randf_range(0.0f, static_cast<float>(length));
                    anim->seek(offset, true);
                }
            }
        }
    }
}

void Seagulls::_bind_methods()
{
}