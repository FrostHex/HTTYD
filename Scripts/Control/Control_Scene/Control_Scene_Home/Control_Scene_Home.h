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
#include "Control_Top.h"

namespace godot
{
    class Control_Main;

    class Control_Scene_Home : public Control_Top
    {
        GDCLASS(Control_Scene_Home, Node);

        public:
            Control_Scene_Home();
            ~Control_Scene_Home();
            void _ready() override;

        protected:
            static void _bind_methods();

        private:
            Control_Main* control_main = nullptr;
            Node* time_of_day = nullptr;
            void _on_button_pressed(const String& scene_name);
            void _on_settings_button_pressed();
            void _on_close_button_pressed();
            void _on_sky_time_changed(double value);
            void _on_setting_enum_changed(int index, const String& prop_name);
            void _on_setting_bool_toggled(bool pressed, const String& prop_name, bool is_custom);
            void _build_settings_entries();
            void _clear_settings_entries();
            void _update_setting_labels();
            void _update_button_texts(); // Helper function to update button texts
            void _update_badge_display(); // Helper function to update badge display
            String _get_json_text(const String& key, const String& fallback = ""); // Helper function to get text from JSON
            Node* viewport_container = nullptr;
            Node* settings_panel = nullptr;
            Node* settings_content = nullptr;
            Node* badge_icon = nullptr; // Badge display node
    };
}

#endif // CONTROL_SCENE_HOME_H