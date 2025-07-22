#ifndef DRAGON_ANIMATOR_H
#define DRAGON_ANIMATOR_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <map>

namespace godot 
{
    class DragonAnimator : public Node // extends the Node class
    {
        GDCLASS(DragonAnimator, Node);

        public:
            DragonAnimator();  // constructor
            ~DragonAnimator(); // destructor
            void _ready() override;
            void _physics_process(float delta);
            void SetAnimation(const String &layer, const String &animation, bool freeze = false);
            void Unfreeze();
            void SetAnimation_Mouth(int step, float thresh);
            void SetAnimation_Weight(const String &layer, float weight);
            
        protected:
            static void _bind_methods();
        
        private:
            AnimationTree* anim_tree = nullptr;
            std::map<String, AnimationNodeStateMachinePlayback*> layer_map;
            AnimationPlayer* anim_player = nullptr;
            std::map<String, double> original_anim_length; // a map to store the original length of animations
            String last_frozen_layer; // the last layer that was frozen
            String last_frozen_animation; // the last animation that was frozen
            int animation_mouth_step = 0; 
            float animation_mouth_thresh = 0.0f;
            float animation_mouth_weight = 1.0f;
    };
}
# endif // DRAGON_ANIMATOR_H