#include "DragonAnimator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_node_state_machine.hpp>

using namespace godot;

// Implement constructor and destructor
DragonAnimator::DragonAnimator() 
{
}

DragonAnimator::~DragonAnimator()
{
}

void DragonAnimator::_bind_methods() 
{
}

/**
 * @brief get animation references, define the layer map, and set the default animation
 */
void DragonAnimator::_ready() 
{
    // retrieve the AnimationTree node and the AnimationPlayer node from the scene tree
    AnimationTree *anim_tree = get_node<AnimationTree>("AnimationTree");
    if (!anim_tree) 
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationTree not found");
        return;
    }
    AnimationPlayer *anim_player = get_parent()->get_node<Node>("Pivot")->get_node<Node>("Toothless")->get_node<AnimationPlayer>("AnimationPlayer");
    if (!anim_player)
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationPlayer not found");
        return;
    }
    
    anim_tree->set_animation_player(anim_player->get_path()); // Assign the animation player to our AnimationTree member via its NodePath

    // get the AnimationNodeStateMachinePlayback nodes from the AnimationTree
    layer_map["base"]  = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_base/playback"));
    layer_map["eye"]   = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_eye/playback"));
    layer_map["shake"] = Object::cast_to<AnimationNodeStateMachinePlayback>(anim_tree->get("parameters/layer_shake/playback"));

    if (!Engine::get_singleton()->is_editor_hint()) // when the game is running
    {
        SetAnimation("base", "po_glide");
        SetAnimation("eye", "po_eye_medium");
        SetAnimation("shake", "lo_shake");
    }
}

/**
 * @brief set the animation for the specified layer
 * @param layer the name of the layer
 * @param animation the name of the animation
 */
void DragonAnimator::SetAnimation(const String &layer, const String &animation) 
{
    auto it = layer_map.find(layer);
    if (it != layer_map.end() && it->second) 
    {
        it->second->travel(animation);
    } 
    else 
    {
        UtilityFunctions::printerr(String("DragonAnimator: Invalid layer name: ") + layer); 
    }
}