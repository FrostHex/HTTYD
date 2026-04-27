#ifndef DRAGON_PILOT_JOYSTICK_H
#define DRAGON_PILOT_JOYSTICK_H

#include "Dragon_Pilot_Top.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/xr_controller3d.hpp>

namespace godot 
{
    class Dragon_Pilot_Joystick : public Dragon_Pilot_Top // extends the Node class
    {
        GDCLASS(Dragon_Pilot_Joystick, Dragon_Pilot_Top);

        public:            
            Dragon_Pilot_Joystick();  // constructor
            ~Dragon_Pilot_Joystick(); // destructor
            void _ready() override;
            void GetInput(float* input_keys) override;
            void _physics_process(double delta) override;

        protected:
            static void _bind_methods();
            void SetMotionAngularCrisis(double delta) override;

        private:
            void RefreshXRControllers();
            XRController3D *hand_left = nullptr;
            XRController3D *hand_right = nullptr;
            bool y_button_prev = false; // edge detection for Y/B button
    };
}

#endif // DRAGON_PILOT_JOYSTICK_H