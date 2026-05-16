#ifndef CONTROL_SCENE_TOP_H
#define CONTROL_SCENE_TOP_H

#include "Dragon_Pilot_Top.h"
#include "Control_Camera.h"
#include "Control_Main.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/input_event.hpp>

namespace godot
{
    class Control_Scene_Top : public Node
    {
        public:
            Control_Scene_Top() = default;
            ~Control_Scene_Top() override = default;
            void _ready();
            void ReturnHome();
            void _input_top(const Ref<InputEvent> &event);

        protected:
            static void _bind_methods();
            void SyncSkyTime(Node *time_of_day);
            Dragon_Pilot_Top* dragon_pilot = nullptr;
            Control_Camera* ctrl_camera = nullptr;
            Control_Main* control_main = nullptr;
            Node3D* camera_main = nullptr;
    };
}

#endif // CONTROL_SCENE_TOP_H
