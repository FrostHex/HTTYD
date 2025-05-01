#ifndef DRAGON_CONTROL_KEYBOARD_H
#define DRAGON_CONTROL_KEYBOARD_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event.hpp> 
#include <godot_cpp/variant/vector3.hpp>    // for Vector3
#include "../DragonControlTop/DragonControlTop.h"
#include <godot_cpp/classes/rigid_body3d.hpp>    // for RigidBody3D

namespace godot 
{
    class DragonControlKeyboard : public DragonControlTop // extends the Node class
    {
        GDCLASS(DragonControlKeyboard, DragonControlTop);

        public:
            DragonControlKeyboard();  // constructor
            ~DragonControlKeyboard(); // destructor
            void _input(const Ref<InputEvent> &event) override;
            // 模板基类接口实现
            float getLinearInput();
            void handleAngular(RigidBody3D *rb);

        protected:
            static void _bind_methods();

        private:
            const float DRAGON_FACTOR_LINEAR = 1.0f;
            const float DRAGON_FACTOR_PITCH = 0.015f;
            const float DRAGON_FACTOR_ROLL = 0.018f;
            const float DRAGON_FACTOR_YAW = 0.3f;
            const float DRAGON_FACTOR_DAMPING = 0.965f;
            const float DRAGON_FACTOR_UPSIDE_DOWN = 1.5f;
            Input *input_singleton;
            bool height_initialized = false;
            float height_init = 0.0f;
            float height_delta = 0.0f;
            float linear_velocity_input = 100.0f;
            float linear_velocity = 0.0f;
            Vector3 angular_velocity_buildup = Vector3(0, 0, 0);
    };
}

#endif // DRAGON_CONTROL_KEYBOARD_H