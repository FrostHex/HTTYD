#ifndef CONTROL_SCENE_TUTORIAL_H
#define CONTROL_SCENE_TUTORIAL_H

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

    class Control_Scene_Tutorial : public Node 
    {
        GDCLASS(Control_Scene_Tutorial, Node)

        public:
            Control_Scene_Tutorial();
            ~Control_Scene_Tutorial() override;
            void _ready() override;
            void _input(const Ref<InputEvent> &event) override;
            void _physics_process(double delta) override;

        protected:
            static void _bind_methods();

        private:
            Timer* timer = nullptr;
            Node3D* tutorial_paper = nullptr;
            Label3D* tutorial_label = nullptr;
            int current_index = 0;
            PackedStringArray lines;
            Control_Main* control_main = nullptr;
            DragonControlTop* dragon_control;
            DragonAnimator* dragon_animator;
            Control_Camera* ctrl_camera;
            Node* camera_main;
            void _on_back_button_pressed();
            String _process_tutorial_text(const String &text);
            // VR A/B 翻页支持
            XRController3D* hand_right = nullptr; // 只需右手：A/B 在右手上
            bool a_button_prev = false;
            bool b_button_prev = false;
            void _goto_next_line();
            void _goto_prev_line();
    };

} // namespace godot

#endif // CONTROL_SCENE_TUTORIAL_H