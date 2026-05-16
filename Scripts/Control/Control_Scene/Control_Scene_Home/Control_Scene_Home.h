# ifndef CONTROL_SCENE_HOME_H
# define CONTROL_SCENE_HOME_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include "Control_Scene_Top.h"

namespace godot
{
    class Settings;
    class Control_Main;
    class Menu;

    class Control_Scene_Home : public Control_Scene_Top
    {
        GDCLASS(Control_Scene_Home, Node);

    public:
        Control_Scene_Home();
        ~Control_Scene_Home();
        void _ready() override;

    private:
        static void _bind_methods();

        // ── 基础引用 ────────────────────────────────────────
        Settings*       settings        = nullptr;
        Menu*           menu            = nullptr;

        Node*   viewport_container  = nullptr;
    };
}

#endif // CONTROL_SCENE_HOME_H