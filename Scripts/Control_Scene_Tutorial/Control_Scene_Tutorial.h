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

namespace godot 
{

    class Control_Main;

    class Control_Scene_Tutorial : public Node 
    {
        GDCLASS(Control_Scene_Tutorial, Node)

    public:
        Control_Scene_Tutorial();
        ~Control_Scene_Tutorial() override;
        void _ready() override;
        void _input(const Ref<InputEvent> &event) override;

    protected:
        static void _bind_methods();

    private:
    Timer *timer = nullptr;
    Label3D *tutorial_label = nullptr;
    int current_index = 0;
    PackedStringArray lines;
    Control_Main* control_main = nullptr;

    void _on_timer_timeout();
    void _on_back_button_pressed();
    };

} // namespace godot

#endif // CONTROL_SCENE_TUTORIAL_H