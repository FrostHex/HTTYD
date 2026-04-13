#ifndef DRAGON_PILOT_TOP_H
#define DRAGON_PILOT_TOP_H

#define DRAGON_FACTOR_LINEAR 3
#define DRAGON_FACTOR_YAW 35
#define DRAGON_FACTOR_PITCH 0.9f
#define DRAGON_FACTOR_ROLL 1.08f
#define DRAGON_FACTOR_DAMPING 0.965f
#define DRAGON_FACTOR_UPSIDE_DOWN 1.5f
#define DRAGON_FACTOR_GLIDE 0.3f
#define DRAGON_CRISIS_P_GAIN 5
#define DRAGON_HIT_CLIFF_HEIGHT 150

#include "Dragon_Animator.h"

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
        STATE_APPROACHING,
        STATE_HIT_CLIFF,
        STATE_FALLING,
        STATE_CRISIS,
        STATE_ROLLING, 
        STATE_DISABLED,
        STATE_COUNT // enum index start from 0, so the value of STATE_COUNT is the number of states above
    };

    class Control_Camera;
    // class GameTimer;

    class Dragon_Pilot_Top : public Node 
    {
        GDCLASS(Dragon_Pilot_Top, Node);

        public:
            Dragon_Pilot_Top();
            ~Dragon_Pilot_Top();
            void _ready();
            void _physics_process(double delta) override; // override the _physics_process function from Node class
            float GetLinearVelocity();
            Dictionary GetStatus();// const: this function does not modify the object         
            void SetStatus(const Dictionary& status);
            void SetState(DragonState new_state);
            DragonState GetState() const; // const: this function does not modify the object
            float time_to_target = 0.1f;
            void SetClearToothlessRotation(bool value);
            void SetTargetRotation(Vector3 target_rotation);
            void SetVelocityAngular(Vector3 angular_velocity);
            void SetStatus_Deferred(const Array& dragon_transform, float linear_velocity_input);

        protected:
            static void _bind_methods();
            void _on_body_entered(Node* body);
            // virtual: this function can be overridden in derived classes
            // =0: pure virtual function, which must be implemented in derived classes
            // the class containing pure virtual functions is an abstract class
            virtual void GetInput(float* input_keys) = 0;
            Input *input_singleton;    
            RigidBody3D *dragon_rb;
            Control_Camera* ctrl_camera; // pointer to the Control_Camera class instance
            float input_keys[3] = {0.0f, 0.0f, 0.0f};
            Vector3 angular_velocity_buildup = Vector3(0, 0, 0);

        private:
            Dragon_Animator* dragon_animator;
            float height_init = 0.0f;
            float height_delta = 0.0f;
            float linear_velocity_input = 100.0f;
            float linear_velocity = 0.0f;
            void SetMotionLinear(double delta);
            void SetMotionAngular(double delta);
            void SetAnimation();
            void SetAnimationCrisis();
            virtual void SetMotionAngularCrisis(double delta) = 0;
            // define a new type name (StateProcessFunc) for the function pointers
            // it represents a pointer to a member function of Dragon_Pilot_Top class that takes a double argument and returns void
            using StateProcessFunc = void (Dragon_Pilot_Top::*)(double);
            StateProcessFunc state_process_funcs[STATE_COUNT]; // an array of function pointers with size of STATE_COUNT
            DragonState state_current;
            void ProcessDefault(double delta);
            void ProcessNotAnimated(double delta);
            void ProcessApproaching(double delta);
            void ProcessHitCliff(double delta);
            void ProcessFalling(double delta);
            void ProcessCrisis(double delta);
            void ProcessRolling(double delta);
            void ProcessDisabled(double delta);
            float p_gain = 0.0f; 
            Node3D* pivot_toothless = nullptr;
            Node3D* camera_main = nullptr;
            Vector3 target_position = Vector3(0, 0, 0);
            Node3D* pillar_hit_1 = nullptr;
            Node3D* pillar_hit_2 = nullptr;
            float cliff_distance_threshold;
            void ApproachTarget(bool setting_angular, bool setting_linear, float* time_to_target, float time_delta, Vector3* target_position, int distance_offset = 0);
            void TriggerApproaching(bool setting_angular, Vector3 target_position, float time_to_target);
            float velocity_clamp = 300.0f;
            bool setting_angular = false;
            bool clear_pivot_rotation = false;
            Vector3 target_rotation = Vector3(0, 0, 0);
            bool approach_target_rotation = false;
            float rolled_angle = 0.0f;
            // GameTimer* timer = nullptr;
    };
}

VARIANT_ENUM_CAST(godot::DragonState); // use godot macro (VARIANT_ENUM_CAST) to register the enum values in the engine

#endif // DRAGON_PILOT_TOP_H