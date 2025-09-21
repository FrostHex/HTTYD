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
#include "DragonAnimator.h"
#include "Control_Camera.h"
#include "DragonControlJoystick.h"
#include "DragonControlKeyboard.h"

namespace godot 
{
    class XRController3D; // forward declaration

    class Control_Main;

    class Control_Scene_Practice : public Node 
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
            DragonControlTop* dragon_control;
            DragonAnimator* dragon_animator;
            Control_Camera* ctrl_camera;
            Node* camera_main;
            // Practice文字显示支持
            Node3D* practice_paper = nullptr;
            Label3D* practice_label = nullptr;
            // VR B 切换状态支持
            XRController3D* hand_right = nullptr;
            bool b_button_prev = false;
            bool a_button_prev = false;
            // 当前龙状态（true=crisis, false=default）
            bool is_crisis_state = false;
            void _on_back_button_pressed();
            void _toggle_dragon_state();
    };

} // namespace godot

#endif // CONTROL_SCENE_PRACTICE_H