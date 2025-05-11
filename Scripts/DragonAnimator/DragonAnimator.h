#ifndef DRAGON_ANIMATOR_H
#define DRAGON_ANIMATOR_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>
#include <map> // 替换 unordered_map

namespace godot 
{
    class DragonAnimator : public Node // extends the Node class
    {
        GDCLASS(DragonAnimator, Node);

        public:
            DragonAnimator();  // constructor
            ~DragonAnimator(); // destructor
            void _ready() override;
            void SetAnimation(const String &layer, const String &animation);
            void SetAnimation(const String &layer, const String &animation, float transition_time);
        
        protected:
            static void _bind_methods();
        
        private:
            std::map<String, AnimationNodeStateMachinePlayback*> layer_map;
    };
}
# endif // DRAGON_ANIMATOR_H