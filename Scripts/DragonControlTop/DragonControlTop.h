#ifndef DRAGON_CONTROL_TOP_H
#define DRAGON_CONTROL_TOP_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include "../DragonControlKeyboard/DragonControlKeyboard.h"

namespace godot {

class DragonControlTop : public Node {
    GDCLASS(DragonControlTop, Node);

protected:
    static void _bind_methods();

public:
    DragonControlTop();
    ~DragonControlTop();

    void _ready() override;

    void set_joystick_input(bool p_val);
    bool is_joystick_input() const;

private:
    bool joystick_input = false;
};

}

#endif // DRAGON_CONTROL_TOP_H
