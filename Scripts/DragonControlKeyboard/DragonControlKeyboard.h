#ifndef SIMPLE_INPUT_H
#define SIMPLE_INPUT_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp> 
#include <godot_cpp/variant/vector3.hpp>    // for Vector3

namespace godot 
{
    class DragonControlKeyboard : public Node // extends the Node class
    {
        GDCLASS(DragonControlKeyboard, Node);

        public:
            DragonControlKeyboard();  // constructor
            ~DragonControlKeyboard(); // destructor
            void _process(double delta) override;
            void _input(const Ref<InputEvent> &event) override;

        protected:
            static void _bind_methods();

        private:
            const real_t DRAGON_FACTOR_PITCH = 0.0012f;
            const real_t DRAGON_FACTOR_ROLL = 0.0014f;
            const real_t DRAGON_FACTOR_LINEAR = 1.0;
            Input *input_singleton;
            bool height_initialized = false;
            double height_init = 0.0;
            double height_delta = 0.0;
            double linear_velocity_input = 100;
            double linear_velocity = 0;
            Vector3 angular_velocity = Vector3(0, 0, 0);
    };
}

#endif // SIMPLE_INPUT_H