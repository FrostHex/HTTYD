# ifndef CONTROL_MAIN_H
# define CONTROL_MAIN_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/viewport.hpp>

namespace godot
{
    class Control_Main : public Node3D // Change made here
    {
        GDCLASS(Control_Main, Node3D); // Change made here

        public:
            Control_Main();
            ~Control_Main();
            void _ready();
            void Switch_Scene(const String &scene_name);
            void SetValJoystickInput(bool val);
            bool GetValJoystickInput() const { return enable_headset; } // the const keyword indicates that this function does not modify the instance variables
            void SetValSubView(bool val) { sub_view = val; }
            bool GetValSubView() const { return sub_view; }
            void SetValDebug(bool val) { debug = val; }
            bool GetValDebug() const { return debug; }

        protected:
            static void _bind_methods();

        private:
            bool enable_headset = false;
            bool sub_view = true;
            bool debug = false; // debug mode
            Node3D* camera_main = nullptr;
    };
}

#endif // CONTROL_MAIN_H