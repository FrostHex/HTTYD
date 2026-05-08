// ==================== Control_Main.h ====================
#ifndef CONTROL_MAIN_H
#define CONTROL_MAIN_H

#include "Control_Camera.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/viewport.hpp>

namespace godot
{
    class Settings;

    class Control_Main : public Node
    {
        GDCLASS(Control_Main, Node);

    public:
        Control_Main();
        ~Control_Main();
        void _ready();
        void Switch_Scene(const String &scene_name);

    protected:
        static void _bind_methods();

    private:
        void AttachCamera(const String &scene_name);
        void AttachSunshineClouds(const String &scene_name, bool attach);

        Control_Camera* ctrl_camera = nullptr;
        Node3D* camera_main = nullptr;
    };
}

#endif // CONTROL_MAIN_H