#ifndef MAIN_CONTROL_H
#define MAIN_CONTROL_H

#include <godot_cpp/classes/node.hpp>
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
            void _ready() override;
            void _input(const Ref<InputEvent> &event) override;
            void SetValJoystickInput(bool p_val);
            bool GetValJoystickInput() const;
            void SetValSubView(bool p_val);
            bool GetValSubView() const;

        protected:
            static void _bind_methods();

        private:
            bool enable_headset = false;
            bool sub_view = true;
    };
}

#endif // MAIN_CONTROL_H