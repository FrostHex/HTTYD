#ifndef SIMPLE_INPUT_H
#define SIMPLE_INPUT_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp> 

namespace godot 
{
    class DragonControlKeyboard : public Node // extends the Node class
    {
        GDCLASS(DragonControlKeyboard, Node);

        public:
            DragonControlKeyboard();  // constructor
            ~DragonControlKeyboard(); // destructor
            void _process(double delta);
            void _input(const Ref<InputEvent> &event);

        protected:
            static void _bind_methods();

        private:
            Input *input_singleton;
    };
}

#endif // SIMPLE_INPUT_H