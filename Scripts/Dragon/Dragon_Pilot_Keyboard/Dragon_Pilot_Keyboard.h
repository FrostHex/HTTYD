#ifndef DRAGON_PILOT_KEYBOARD_H
#define DRAGON_PILOT_KEYBOARD_H

#include "Dragon_Pilot_Top.h"
#include <godot_cpp/classes/input_event.hpp>

namespace godot 
{
    class Dragon_Pilot_Keyboard : public Dragon_Pilot_Top // extends the Node class
    {
        GDCLASS(Dragon_Pilot_Keyboard, Dragon_Pilot_Top);

        public:            
            Dragon_Pilot_Keyboard();  // constructor
            ~Dragon_Pilot_Keyboard(); // destructor
            void GetInput(float* input_keys) override;
            void _input(const Ref<InputEvent> &event) override;

        protected:
            static void _bind_methods();
            void SetMotionAngularCrisis(double delta) override;

        private:
    };
}

#endif // DRAGON_PILOT_KEYBOARD_H