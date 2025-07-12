#include "DragonAnimator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
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
    ClassDB::bind_method(D_METHOD("SetAnimation", "layer", "animation", "freeze"), &DragonAnimator::SetAnimation, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Unfreeze"), &DragonAnimator::Unfreeze);
}


/**
 * @brief get animation references, define the layer map, and set the default animation
 */
void DragonAnimator::_ready() 
{
    // retrieve the AnimationTree node and the AnimationPlayer node from the scene tree
    AnimationTree *anim_tree = get_node<AnimationTree>("AnimationTree");
    anim_player = get_parent()->get_node<Node>("Pivot")->get_node<Node>("Toothless")->get_node<AnimationPlayer>("AnimationPlayer");
    anim_tree->set_animation_player(anim_player->get_path()); // Assign the animation player to our AnimationTree member via its NodePath

    // get the AnimationNodeStateMachinePlayback nodes from the AnimationTree
    layer_map["layer_wing_main"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_wing_main/playback"));
    layer_map["layer_wing_tail"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_wing_tail/playback"));
    layer_map["layer_eye_shape"]   = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_eye_shape/playback"));
    layer_map["layer_shake"] = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_shake/playback"));

    if (!Engine::get_singleton()->is_editor_hint()) // when the game is running
    {
        SetAnimation("layer_shake", "lo_shake");
    }

    // anim_tree->set("parameters/add_eyelid_up/add_amount", 0.9); // Set the blend weight for the add_eyelid_up add2 node
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