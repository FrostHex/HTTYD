#ifndef DRAGON_CONTROL_JOYSTICK_H
#define DRAGON_CONTROL_JOYSTICK_H

#include "DragonControlTop.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/xr_controller3d.hpp>

namespace godot 
{
    class DragonControlJoystick : public DragonControlTop // extends the Node class
    {
        GDCLASS(DragonControlJoystick, DragonControlTop);

        public:            
            DragonControlJoystick();  // constructor
            ~DragonControlJoystick(); // destructor
            void _ready() override;
            void GetInput(float* input_keys) override;
            void _physics_process(double delta) override;

        protected:
            static void _bind_methods();
            void SetMotionAngularCrisis(double delta) override;

        private:
            XRController3D *hand_left = nullptr;
            XRController3D *hand_right = nullptr;
            bool y_button_prev = false; // edge detection for Y/B button
    };
}

#endif // DRAGON_CONTROL_JOYSTICK_H