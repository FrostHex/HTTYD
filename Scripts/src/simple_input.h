#ifndef SIMPLE_INPUT_H
#define SIMPLE_INPUT_H

#include <godot_cpp/classes/node.hpp>    // 基础类Node
#include <godot_cpp/core/class_db.hpp>   // 用于注册类
#include <godot_cpp/core/binder_common.hpp>

namespace godot {

class SimpleInput : public Node {
    GDCLASS(SimpleInput, Node);

protected:
    static void _bind_methods();  // 注册方法到Godot用

public:
    SimpleInput();  // 构造函数
    ~SimpleInput(); // 析构函数

    void _process(double delta); // 重写process函数
};

}

#endif // SIMPLE_INPUT_H
