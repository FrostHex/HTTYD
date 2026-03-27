#include "DragonAnimator.h"

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
DragonAnimator::DragonAnimator() 
{
}


/**
 * @brief destructor
 */
DragonAnimator::~DragonAnimator()
{
}


void DragonAnimator::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("RefreshBindings"), &DragonAnimator::RefreshBindings);
    ClassDB::bind_method(D_METHOD("SetAnimation", "layer", "animation", "freeze"), &DragonAnimator::SetAnimation, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Unfreeze"), &DragonAnimator::Unfreeze);
    ClassDB::bind_method(D_METHOD("SetAnimation_Mouth", "step", "thresh"), &DragonAnimator::SetAnimation_Mouth);
    ClassDB::bind_method(D_METHOD("SetAnimation_Weight", "layer", "weight"), &DragonAnimator::SetAnimation_Weight);
}


/**
 * @brief get animation references, define the layer map, and set the default animation
 */
void DragonAnimator::_ready() 
{
    RefreshBindings();
}


void DragonAnimator::RefreshBindings()
{
    Node *species_slot = get_parent()->get_node_or_null("SpeciesSlot");
    if (!species_slot)
    {
        UtilityFunctions::printerr("DragonAnimator: SpeciesSlot not found.");
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
        UtilityFunctions::printerr("DragonAnimator: No dragon root found under SpeciesSlot.");
        return;
    }

    anim_tree = dragon_root->get_node<AnimationTree>("AnimationTree");
    if (!anim_tree)
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationTree not found on dragon root.");
        return;
    }
    anim_tree->set_active(true);

    Node *model_root = dragon_root->get_node_or_null("Model");
    if (!model_root || model_root->get_child_count() == 0)
    {
        UtilityFunctions::printerr("DragonAnimator: Model node missing or empty.");
        return;
    }

    anim_player = model_root->get_child(0)->get_node<AnimationPlayer>("AnimationPlayer");
    if (!anim_player)
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationPlayer not found.");
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
                // anim_tree->set("parameters/add_wing_main/add_amount", 1.0);
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
void DragonAnimator::SetAnimation(const String &layer, const String &animation, bool freeze)
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
        UtilityFunctions::printerr(String("DragonAnimator: Invalid layer name: ") + layer); 
    }
}


void DragonAnimator::Unfreeze()
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
void DragonAnimator::SetAnimation_Mouth(int step, float thresh)
{
    animation_mouth_step = step; 
    animation_mouth_thresh = thresh;
    set_physics_process(true);
}


void DragonAnimator::_physics_process(float delta)
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


void DragonAnimator::SetAnimation_Weight(const String &layer, float weight)
{
    if (!anim_tree)
    {
        UtilityFunctions::printerr("DragonAnimator: anim_tree is null when setting weight.");
        return;
    }
    anim_tree->set("parameters/" + layer + "/add_amount", weight);
}