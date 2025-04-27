#include "DragonAnimator.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>

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

void DragonAnimator::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }
    AnimationTree *anim_tree =  get_parent()->get_node<AnimationTree>("AnimationTree");
    if (!anim_tree) 
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationTree not found");
        return;
    }
    // Retrieve the AnimationPlayer node from the scene tree
    AnimationPlayer *anim_player = get_parent()->get_node<Node>("Pivot")
                                  ->get_node<Node>("Toothless")->get_node<AnimationPlayer>("AnimationPlayer");

    if (!anim_player)
    {
        UtilityFunctions::printerr("DragonAnimator: AnimationPlayer not found");
        return;
    }

    // Assign the animation player to our AnimationTree member via its NodePath
    anim_tree->set_animation_player(anim_player->get_path());

    // 获取状态机播放接口
    Variant playback_var = anim_tree->get("parameters/playback");
    AnimationNodeStateMachinePlayback *playback =
        Object::cast_to<AnimationNodeStateMachinePlayback>(playback_var);
    
    if (playback) {
        playback->travel("lo_up"); // 切换到 lo_up 动画
    } else {
        UtilityFunctions::printerr("DragonAnimator: 无法获取 StateMachinePlayback");
    }
}