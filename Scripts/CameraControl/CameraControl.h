#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

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
#include "DragonControlKeyboard.h"
#include "GameTimer.h"

namespace godot 
{
    class CameraControl : public Node // extends the Node class
    {
        GDCLASS(CameraControl, Node);

        public:
            CameraControl(bool sub_view = true, bool enable_headset = false);
            ~CameraControl(); // destructor
            void _ready();
            void _physics_process(double delta) override;
            void _input(const Ref<InputEvent> &event) override;
            Vector3 GetPostureHeadset();
            void SetDragonControl(DragonControlTop* dragon_control);
            String info_debug;
            String time_elapsed;
        
        protected:
            static void _bind_methods();
        
        private:
            void Print_Collision(Node* body, float velocity);
            bool sub_view = true; // whether to use the sub camera
            bool enable_headset = false; // whether to use the headset
            bool xr_position_initialized = false; // whether XR position has been initialized
            Vector3 initial_origin_position; // initial XR origin position
            Quaternion initial_origin_rotation; // initial XR origin rotation
            Quaternion initial_camera_rotation; // initial XR camera rotation
            Camera3D *camera_sub = nullptr;
            Label* label_info = nullptr;
            DragonControlTop* dragon_control = nullptr;
            RigidBody3D *dragon_rb = nullptr;
            Node3D *xr_node = nullptr;
            Node3D *xr_origin = nullptr;
            Node3D *xr_camera = nullptr;
    };
}

#endif // CAMERA_CONTROL_H