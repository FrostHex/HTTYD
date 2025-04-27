#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include <godot_cpp/classes/node.hpp>       // base class Node
#include <godot_cpp/core/class_db.hpp>      // used for class registration
#include <godot_cpp/core/binder_common.hpp> // used for binding methods and properties
#include <godot_cpp/classes/camera3d.hpp>   // 引入 Camera3D 类型

namespace godot 
{
    class CameraControl : public Node // extends the Node class
    {
        GDCLASS(CameraControl, Node);

        public:
            CameraControl(bool sub_view = true);  // constructor，支持无参调用
            ~CameraControl(); // destructor
            void _ready();
            void _physics_process(double delta) override;
        
        protected:
            static void _bind_methods();
        
        private:
            bool sub_view = true; // 是否使用子视图
            Camera3D *camera_sub = nullptr;
    };
}

#endif // CAMERA_CONTROL_H