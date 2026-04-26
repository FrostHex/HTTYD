#ifndef CONTROL_SCENE_PRACTICE_H
#define CONTROL_SCENE_PRACTICE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include "Dragon_Animator.h"
#include "Control_Camera.h"
#include "Control_Top.h"
#include "Dragon_Pilot_Joystick.h"
#include "Dragon_Pilot_Keyboard.h"

namespace godot 
{
    class XRController3D; // forward declaration

    class Control_Main;

    class Control_Scene_Practice : public Control_Top 
    {
        GDCLASS(Control_Scene_Practice, Node)

        public:
            Control_Scene_Practice();
            ~Control_Scene_Practice() override;
            void _ready() override;
            void _input(const Ref<InputEvent> &event) override;
            void _physics_process(double delta) override;

        protected:
            static void _bind_methods();

        private:
            int current_index = 0;
            PackedStringArray lines;
            Control_Main* control_main = nullptr;
            Dragon_Pilot_Top* dragon_pilot = nullptr;
            Dragon_Animator* dragon_animator = nullptr;
            Control_Camera* ctrl_camera = nullptr;
            Node3D* camera_main = nullptr;
            // Practice text display support.
            Node3D* practice_paper = nullptr;
            Label3D* practice_label = nullptr;
            // VR B state toggle support.
            XRController3D* hand_right = nullptr;
            bool b_button_prev = false;
            bool a_button_prev = false;
            // current dragon state (true = crisis, false = default).
            bool is_crisis_state = false;
            void _on_back_button_pressed();
            void _toggle_dragon_state();
    };

} // namespace godot

#endif // CONTROL_SCENE_PRACTICE_H