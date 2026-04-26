#ifndef CONTROL_CAMERA_H
#define CONTROL_CAMERA_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/variant/vector3.hpp>    // for Vector3
#include <godot_cpp/variant/quaternion.hpp> // for Quaternion
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/remote_transform3d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/video_stream_player.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include "Dragon_Pilot_Keyboard.h"
#include "GameTimer.h"

namespace godot 
{
    class Control_Main; // Forward declaration

    class Control_Camera : public Node // extends the Node class
    {
        GDCLASS(Control_Camera, Node);

        public:
            Control_Camera();
            ~Control_Camera(); // destructor
            void _ready();
            void _physics_process(double delta) override;
            void _input(const Ref<InputEvent> &event) override;
            void Initialize();
            void ResetVRTransform();
            Vector3 GetPostureHeadset();
            void SetDragon_Pilot_(Dragon_Pilot_Top* dragon_pilot);
            void GrabSaddle();
            void ReparentCamera(const NodePath &target_path);
            String info_debug;
            String time_elapsed;
            void SetCameraStabilized(bool stabilized) {camera_stabilized = stabilized;}
            Node3D* camera_main = nullptr;
        
        protected:
            static void _bind_methods();
        
        private:
            void Print_Collision(Node* body, float velocity);
            Control_Main* control_main = nullptr; // reference to Control_Main for accessing shared variables
            Camera3D *camera_sub = nullptr;
            Label* label_info = nullptr;
            Dragon_Pilot_Top* dragon_pilot = nullptr;
            RigidBody3D *dragon_rb = nullptr;
            Node3D *xr_node = nullptr;
            Node3D *xr_origin = nullptr;
            Node3D *xr_camera = nullptr;
            float camera_offset_factor = 0.0f;
            void SetCameraOffsetFactor(float factor);
            void TriggerApproachingAngle(Vector3 target_rotation, float p_gain);
            bool approaching_angle = false;
            Vector3 target_rotation;
            float p_gain = 0.0f;
            void TriggerApproachingPosition(Vector3 target_position_offset);
            Vector3 target_position_offset;
            bool approaching_position = false;
            float resetting_transform_time = -1.0f;
            float timer = 0.0f;
            bool camera_stabilized = false;
            bool vr_recenter_pending = false;

    };
}

#endif // CONTROL_CAMERA_H