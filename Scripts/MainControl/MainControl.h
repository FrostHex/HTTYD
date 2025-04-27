#ifndef DRAGON_CONTROL_TOP_H
#define DRAGON_CONTROL_TOP_H

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
            void SetValJoystickInput(bool p_val);
            bool GetValJoystickInput() const;

        protected:
            static void _bind_methods();

        private:
            bool joystick_input = false;
    };
}

#endif // DRAGON_CONTROL_TOP_H