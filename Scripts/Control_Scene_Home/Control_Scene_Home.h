# ifndef CONTROL_SCENE_HOME_H
# define CONTROL_SCENE_HOME_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>

namespace godot
{
    class Control_Main;

    class Control_Scene_Home : public Node3D
    {
        GDCLASS(Control_Scene_Home, Node3D);

        public:
            Control_Scene_Home();
            ~Control_Scene_Home();
            void _ready();

        protected:
            static void _bind_methods();

        private:
            Control_Main* control_main = nullptr;
            void _on_button_pressed(const String& scene_name);
            void _on_settings_button_pressed();
            void _on_close_button_pressed();
            void _on_language_changed(int index);
            void _on_enable_headset_toggled(bool pressed);
            void _on_sub_view_toggled(bool pressed);
            void _on_debug_toggled(bool pressed);
            void _update_button_texts(); // Helper function to update button texts
            String _get_json_text(const String& key, const String& fallback = ""); // Helper function to get text from JSON
            Node* viewport_container = nullptr;
            Node* settings_panel = nullptr;
    };
}

#endif // CONTROL_SCENE_HOME_H