#ifndef DRAGON_CONTROL_TOP_H
#define DRAGON_CONTROL_TOP_H

#define DRAGON_FACTOR_LINEAR 3
#define DRAGON_FACTOR_YAW 18
#define DRAGON_FACTOR_PITCH 0.9f
#define DRAGON_FACTOR_ROLL 1.08f
#define DRAGON_FACTOR_DAMPING 0.965f
#define DRAGON_FACTOR_UPSIDE_DOWN 1.5f

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/input.hpp>

namespace godot 
{
    class DragonControlTop : public Node 
    {
        GDCLASS(DragonControlTop, Node);

        public:
            DragonControlTop();
            ~DragonControlTop();
            void _ready();
            void _physics_process(double delta) override; // override the _physics_process function from Node class

        protected:
            static void _bind_methods();
            // virtual: this function can be overridden in derived classes
            // =0: pure virtual function, which must be implemented in derived classes
            // the class containing pure virtual functions is an abstract class
            virtual void GetInput(float* input_keys) = 0;
            Input *input_singleton;

        private:
            RigidBody3D *dragon_rb;
            float input_keys[3] = {0.0f, 0.0f, 0.0f};
            float height_init = 0.0f;
            float height_delta = 0.0f;
            float linear_velocity_input = 100.0f;
            float linear_velocity = 0.0f;
            Vector3 angular_velocity_buildup = Vector3(0, 0, 0);
            void SetMotionLinear(double delta);
            void SetMotionAngular(double delta);
    };
}

#endif // DRAGON_CONTROL_TOP_H