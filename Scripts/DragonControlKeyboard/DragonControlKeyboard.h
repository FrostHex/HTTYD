#ifndef SIMPLE_INPUT_H
#define SIMPLE_INPUT_H

#include <godot_cpp/classes/node.hpp>    // 基础类Node
#include <godot_cpp/classes/input.hpp>   // 缓存输入单例
#include <godot_cpp/classes/input_event.hpp> // 完整定义 InputEvent
#include <godot_cpp/core/class_db.hpp>   // 用于注册类
#include <godot_cpp/core/binder_common.hpp>

namespace godot {

class DragonControlKeyboard : public Node {
    GDCLASS(DragonControlKeyboard, Node);

protected:
    static void _bind_methods();

public:
    DragonControlKeyboard();  // 构造函数
    ~DragonControlKeyboard(); // 析构函数

    void _process(double delta);
    void _input(const Ref<InputEvent> &event);

private:
    Input *input_singleton;
};

}

#endif // SIMPLE_INPUT_H
