#ifndef DRAGON_CONTROL_JOYSTICK_H
#define DRAGON_CONTROL_JOYSTICK_H

#include "DragonControlTop.h"

namespace godot 
{
    class DragonJoystick : public DragonControlTop // extends the Node class
    {
        GDCLASS(DragonJoystick, DragonControlTop);

        public:            
            DragonJoystick();  // constructor
            ~DragonJoystick(); // destructor
            void GetInput(float* input_keys) override;

        protected:
            static void _bind_methods();

        private:
    };
}

#endif // DRAGON_CONTROL_JOYSTICK_H