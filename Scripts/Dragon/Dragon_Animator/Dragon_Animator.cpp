#include "Dragon_Animator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_node_state_machine.hpp>

using namespace godot;


/**
 * @brief constructor
 */
Dragon_Animator::Dragon_Animator() 
{
}


/**
 * @brief destructor
 */
Dragon_Animator::~Dragon_Animator()
{
}


void Dragon_Animator::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("RefreshBindings"), &Dragon_Animator::RefreshBindings);
    ClassDB::bind_method(D_METHOD("SetAnimation", "layer", "animation", "freeze"), &Dragon_Animator::SetAnimation, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Unfreeze"), &Dragon_Animator::Unfreeze);
    ClassDB::bind_method(D_METHOD("SetAnimation_Mouth", "step", "thresh"), &Dragon_Animator::SetAnimation_Mouth);
    ClassDB::bind_method(D_METHOD("SetAnimation_Weight", "layer", "weight"), &Dragon_Animator::SetAnimation_Weight);
}


/**
 * @brief get animation references, define the layer map, and set the default animation
 */
void Dragon_Animator::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        set_physics_process(false);
        return;
    }
    else
    {
        call_deferred("RefreshBindings"); // defer to avoid timing issues with node setup in the scene tree
    }
}


void Dragon_Animator::RefreshBindings()
{
    Node *species_slot = get_parent()->get_node_or_null("SpeciesSlot");
    if (!species_slot)
    {
        UtilityFunctions::printerr("Dragon_Animator: SpeciesSlot not found.");
        return;
    }

    // Prefer explicit names to avoid queue_free/deferred add timing issues.
    Node *dragon_root = Object::cast_to<Node>(species_slot->get_node_or_null("GronckleRoot"));
    if (!dragon_root)
    {
        dragon_root = Object::cast_to<Node>(species_slot->get_node_or_null("ToothlessRoot"));
    }
    if (!dragon_root && species_slot->get_child_count() > 0)
    {
        dragon_root = Object::cast_to<Node>(species_slot->get_child(0));
    }
    if (!dragon_root)
    {
        UtilityFunctions::printerr("Dragon_Animator: No dragon root found under SpeciesSlot.");
        return;
    }

    anim_tree = dragon_root->get_node<AnimationTree>("AnimationTree");
    if (!anim_tree)
    {
        UtilityFunctions::printerr("Dragon_Animator: AnimationTree not found on dragon root.");
        return;
    }
    anim_tree->set_active(true);

    Node *model_root = dragon_root->get_node_or_null("Model");
    if (!model_root || model_root->get_child_count() == 0)
    {
        UtilityFunctions::printerr("Dragon_Animator: Model node missing or empty.");
        return;
    }

    anim_player = model_root->get_child(0)->get_node<AnimationPlayer>("AnimationPlayer");
    if (!anim_player)
    {
        UtilityFunctions::printerr("Dragon_Animator: AnimationPlayer not found.");
        return;
    }

    layer_map.clear();

    if (dragon_root->get_name().begins_with("Toothless"))
    {
        dragon_species = SPECIES_TOOTHLESS;
        layer_map["layer_wing_main"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_wing_main/playback"));
        layer_map["layer_wing_tail"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_wing_tail/playback"));
        layer_map["layer_mouth"]      = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_mouth/playback"));
        layer_map["layer_eye_shape"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_eye_shape/playback"));
        layer_map["layer_shake"]      = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_shake/playback"));
    }
    else if (dragon_root->get_name().begins_with("Gronckle"))
    {
        dragon_species = SPECIES_GRONCKLE;
        layer_map["add_wing_main"] = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/add_wing_main/playback"));
    }

    if (!Engine::get_singleton()->is_editor_hint()) // when the game is running
    {
        set_physics_process(false);
        switch (dragon_species)
        {
            case SPECIES_TOOTHLESS:
                SetAnimation("layer_shake", "lo_shake");
                break;
            case SPECIES_GRONCKLE:
                SetAnimation_Weight("add_wing_main", 1.0);
                break;
        }
    }

    set_physics_process(false); // disable physics processing by default
}


/**
 * @brief set the animation for the specified layer
 * @param layer the name of the layer
 * @param animation the name of the animation
 */
void Dragon_Animator::SetAnimation(const String &layer, const String &animation, bool freeze)
{
    auto it = layer_map.find(layer);
    if (it != layer_map.end() && it->second) 
    {
        it->second->travel(animation);

        if (freeze) 
        {
            if (anim_player && anim_player->has_animation(animation)) 
            {
                Ref<Animation> anim = anim_player->get_animation(animation);
                if (anim.is_valid()) 
                {
                    original_anim_length[animation] = anim->get_length(); // save the original length
                    anim->set_length(0.0001);
                    last_frozen_layer = layer;
                    last_frozen_animation = animation;
                }
            }
        } 
    }
    else 
    {
        UtilityFunctions::printerr(String("Dragon_Animator: Invalid layer name: ") + layer); 
    }
}


void Dragon_Animator::Unfreeze()
{
    if (last_frozen_animation.is_empty() || last_frozen_layer.is_empty())
    {
        return;
    }

    auto it = layer_map.find(last_frozen_layer);
    if (it != layer_map.end() && it->second && anim_player && anim_player->has_animation(last_frozen_animation)) 
    {
        Ref<Animation> anim = anim_player->get_animation(last_frozen_animation);
        if (anim.is_valid()) 
        {
            if (original_anim_length.count(last_frozen_animation) > 0) 
            {
                anim->set_length(original_anim_length[last_frozen_animation]); // restore the original length
            }
        }
        it->second->stop(); // stop it in order to play it from the beginning
        it->second->start(last_frozen_animation);
    }
    last_frozen_layer = "";
    last_frozen_animation = "";
}


/**
 * @brief Set the animation mouth step
 * @param step negative means open the mouth; higher the absolute value means faster
 * @param thresh the threshold to stop the animation, [0.0, 1.0]
 */
void Dragon_Animator::SetAnimation_Mouth(int step, float thresh)
{
    animation_mouth_step = step; 
    animation_mouth_thresh = thresh;
    set_physics_process(true);
}


void Dragon_Animator::_physics_process(float delta)
{
    animation_mouth_weight += animation_mouth_step * delta;
    if (animation_mouth_step < 0 && animation_mouth_weight <= animation_mouth_thresh) 
    {
        animation_mouth_weight = animation_mouth_thresh; // ensure the weight does not go below the threshold
        set_physics_process(false);
    }
    else if (animation_mouth_step > 0 && animation_mouth_weight >= animation_mouth_thresh) 
    {
        animation_mouth_weight = animation_mouth_thresh; // ensure the weight does not go above the threshold
        set_physics_process(false);
    }
    anim_tree->set("parameters/add_mouth/add_amount", animation_mouth_weight);
}


void Dragon_Animator::SetAnimation_Weight(const String &layer, float weight)
{
    if (!anim_tree)
    {
        RefreshBindings();
        if (!anim_tree)
        {
            UtilityFunctions::printerr("Dragon_Animator: anim_tree is null when setting weight.");
            return;
        }
    }
    anim_tree->set("parameters/" + layer + "/add_amount", weight);
}