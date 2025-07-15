#ifndef DRAGON_CONTROL_KEYBOARD_H
#define DRAGON_CONTROL_KEYBOARD_H

#include "DragonControlTop.h"

namespace godot 
{
    class DragonControlKeyboard : public DragonControlTop // extends the Node class
    {
        GDCLASS(DragonControlKeyboard, DragonControlTop);

        public:            
            DragonControlKeyboard();  // constructor
            ~DragonControlKeyboard(); // destructor
            void GetInput(float* input_keys) override;

        protected:
            static void _bind_methods();
            void SetMotionAngularCrisis(double delta) override;

        private:
    };
}

#endif // DRAGON_CONTROL_KEYBOARD_H