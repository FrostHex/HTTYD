# ifndef MAIN_CONTROL_H
# define MAIN_CONTROL_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot
{
    class MainControl : public Node
    {
        GDCLASS(MainControl, Node);

        public:
            MainControl();
            ~MainControl();
            void _ready();

        protected:
            static void _bind_methods();

        private:
            void Switch_Scene(const String &scene_name);
            Node3D* camera_main;
    };
}

#endif // MAIN_CONTROL_H