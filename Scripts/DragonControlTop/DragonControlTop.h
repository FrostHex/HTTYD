#ifndef DRAGON_CONTROL_TOP_H
#define DRAGON_CONTROL_TOP_H

#define DRAGON_FACTOR_LINEAR 3
#define DRAGON_FACTOR_YAW 35
#define DRAGON_FACTOR_PITCH 0.9f
#define DRAGON_FACTOR_ROLL 1.08f
#define DRAGON_FACTOR_DAMPING 0.965f
#define DRAGON_FACTOR_UPSIDE_DOWN 1.5f
#define DRAGON_FACTOR_GLIDE 0.3f
#define DRAGON_CRISIS_P_GAIN 5

#include "DragonAnimator.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/input.hpp>
#include <functional>

namespace godot 
{
    enum DragonState 
    {
        STATE_DEFAULT,
        STATE_NOT_ANIMATED,
        STATE_HIT_CLIFF,
        STATE_FALLING,
        STATE_CRISIS,
        STATE_DISABLED,
        STATE_COUNT // enum index start from 0, so the value of STATE_COUNT is the number of states above
    };

    class CameraControl;

    class DragonControlTop : public Node 
    {
        GDCLASS(DragonControlTop, Node);

        public:
            DragonControlTop();
            ~DragonControlTop();
            void _ready();
            void _physics_process(double delta) override; // override the _physics_process function from Node class
            float GetLinearVelocity();
            Dictionary GetStatus();// const: this function does not modify the object         
            void SetStatus(const Dictionary& status);
            void SetState(DragonState new_state);
            DragonState GetState() const; // const: this function does not modify the object

        protected:
            static void _bind_methods();
            void _on_body_entered(Node* body);
            // virtual: this function can be overridden in derived classes
            // =0: pure virtual function, which must be implemented in derived classes
            // the class containing pure virtual functions is an abstract class
            virtual void GetInput(float* input_keys) = 0;
            void SetStatus_Deferred(const Array& dragon_transform, float linear_velocity_input);
            Input *input_singleton;    private:
            RigidBody3D *dragon_rb;
            DragonAnimator* dragon_animator;
            float input_keys[3] = {0.0f, 0.0f, 0.0f};
            float height_init = 0.0f;
            float height_delta = 0.0f;
            float linear_velocity_input = 100.0f;
            float linear_velocity = 0.0f;
            Vector3 angular_velocity_buildup = Vector3(0, 0, 0);
            void SetMotionLinear(double delta);
            void SetMotionAngular(double delta);
            void SetMotionAngularCrisis(double delta);
            void SetAnimation();
            void SetAnimationCrisis();
            // define a new type name (StateProcessFunc) for the function pointers
            // it represents a pointer to a member function of DragonControlTop class that takes a double argument and returns void
            using StateProcessFunc = void (DragonControlTop::*)(double);
            StateProcessFunc state_process_funcs[STATE_COUNT]; // an array of function pointers with size of STATE_COUNT
            DragonState state_current;
            void ProcessDefault(double delta);
            void ProcessNotAnimated(double delta);
            void ProcessHitCliff(double delta);
            void ProcessFalling(double delta);
            void ProcessCrisis(double delta);
            void ProcessDisabled(double delta);
            // Vector3 headset_vector_up;
            CameraControl* camera_ctrl; // pointer to the CameraControl class instance
    };
}

VARIANT_ENUM_CAST(godot::DragonState); // use godot macro (VARIANT_ENUM_CAST) to register the enum values in the engine

#endif // DRAGON_CONTROL_TOP_H