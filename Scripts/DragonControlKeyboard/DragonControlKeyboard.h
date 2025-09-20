#ifndef DRAGON_CONTROL_KEYBOARD_H
#define DRAGON_CONTROL_KEYBOARD_H

#include "DragonControlTop.h"
#include <godot_cpp/classes/input_event.hpp>

namespace godot 
{
    class DragonControlKeyboard : public DragonControlTop // extends the Node class
    {
        GDCLASS(DragonControlKeyboard, DragonControlTop);

        public:            
            DragonControlKeyboard();  // constructor
            ~DragonControlKeyboard(); // destructor
            void GetInput(float* input_keys) override;
            void _input(const Ref<InputEvent> &event) override;

        protected:
            static void _bind_methods();
            void SetMotionAngularCrisis(double delta) override;

        private:
    };
}

#endif // DRAGON_CONTROL_KEYBOARD_H