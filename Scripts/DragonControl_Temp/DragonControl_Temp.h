#ifndef DRAGON_CONTROL_TEMP_H
#define DRAGON_CONTROL_TEMP_H

#define GRONCKLE_LINEAR_SPEED 50.0f
#define GRONCKLE_ANGULAR_SPEED 2.0f
#define CAMERA_MOUSE_SENSITIVITY 0.002f

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/node3d.hpp>

namespace godot 
{
    class DragonControl_Temp : public Node 
    {
        GDCLASS(DragonControl_Temp, Node);

        public:
            DragonControl_Temp();
            ~DragonControl_Temp();
            void _ready() override;
            void _physics_process(double delta) override;
            void _input(const Ref<InputEvent> &event) override;

        protected:
            static void _bind_methods();

        private:
            RigidBody3D* gronckle_rb = nullptr;
            Input* input_singleton = nullptr;
            Node3D* camera_main = nullptr;
            float cam_yaw = 0.0f;
            float cam_pitch = 0.0f;
    };
}

#endif // DRAGON_CONTROL_TEMP_H
