#ifndef DRAGON_ANIMATOR_H
#define DRAGON_ANIMATOR_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties

namespace godot 
{
    class DragonAnimator : public Node // extends the Node class
    {
        GDCLASS(DragonAnimator, Node);

        public:
            DragonAnimator();  // constructor
            ~DragonAnimator(); // destructor
            void _ready() override;
        
        protected:
            static void _bind_methods();
        
        private:
    };
}
# endif // DRAGON_ANIMATOR_H