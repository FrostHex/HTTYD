#ifndef CONTROL_SCENE_TUTORIAL_H
#define CONTROL_SCENE_TUTORIAL_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "Dragon_Animator.h"
#include "Control_Scene_Top.h"
#include "Dragon_Pilot_Joystick.h"
#include "Dragon_Pilot_Keyboard.h"

namespace godot 
{
    class XRController3D; // forward declaration

    class Control_Main;

    class Control_Scene_Tutorial : public Control_Scene_Top 
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
            Dragon_Animator* dragon_animator = nullptr;
            String _process_tutorial_text(const String &text);
            // VR A/B page-turn support.
            XRController3D* hand_right = nullptr; // only the right hand is needed: A/B are on the right controller.
            bool a_button_prev = false;
            bool b_button_prev = false;
            void _goto_next_line();
            void _goto_prev_line();
    };

} // namespace godot

#endif // CONTROL_SCENE_TUTORIAL_H