#include "DragonAnimator_Temp.h"

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
DragonAnimator_Temp::DragonAnimator_Temp() 
{
}


/**
 * @brief destructor
 */
DragonAnimator_Temp::~DragonAnimator_Temp()
{
}


void DragonAnimator_Temp::_bind_methods() 
{

}


/**
 * @brief get animation references, define the layer map, and set the default animation
 */
void DragonAnimator_Temp::_ready() 
{
    // retrieve the AnimationTree node and the AnimationPlayer node from the scene tree
    anim_tree = get_node<AnimationTree>("AnimationTree");
    anim_player = get_parent()->get_node<Node>("Gronckle")->get_node<AnimationPlayer>("AnimationPlayer");
    anim_tree->set_animation_player(anim_player->get_path()); // Assign the animation player to our AnimationTree member via its NodePath

    if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

    anim_tree->set("parameters/add_wing_main/add_amount", 1.0);
}
