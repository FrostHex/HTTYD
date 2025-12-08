#ifndef DRAGON_ANIMATOR_TEMP_H
#define DRAGON_ANIMATOR_TEMP_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <map>

namespace godot 
{
    class DragonAnimator_Temp : public Node // extends the Node class
    {
        GDCLASS(DragonAnimator_Temp, Node);

        public:
            DragonAnimator_Temp();  // constructor
            ~DragonAnimator_Temp(); // destructor
            void _ready() override;
                        
        protected:
            static void _bind_methods();
        
        private:
            AnimationTree* anim_tree = nullptr;
            AnimationPlayer* anim_player = nullptr;

    };
}
# endif // DRAGON_ANIMATOR_TEMP_H