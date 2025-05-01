#ifndef DRAGON_CONTROL_TOP_H
#define DRAGON_CONTROL_TOP_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <cmath>

namespace godot {

// 通用物理处理逻辑模板，调用 getLinearInput() 和 handleAngular()
template<typename Derived>
class DragonControlLogic {
    bool height_initialized = false;
    float height_init = 0.0f;
    float height_delta = 0.0f;
    float linear_velocity_input = 100.0f;
    float linear_velocity = 0.0f;
    Vector3 angular_velocity_buildup = Vector3(0,0,0);
public:
    void process(Derived* self, double delta) {
        RigidBody3D *dragon_rb = Object::cast_to<RigidBody3D>(self->get_parent());
        if (!dragon_rb) { UtilityFunctions::printerr("DragonControlLogic: parent not RigidBody3D"); return; }
        if (!height_initialized) { height_init = dragon_rb->get_global_transform().origin.y; height_initialized = true; }
        float input_val = self->getLinearInput();
        linear_velocity_input += input_val * delta;
        height_delta = height_init - dragon_rb->get_global_transform().origin.y;
        linear_velocity = linear_velocity_input + std::copysign(1.0f, height_delta) * std::sqrt(19.6f * std::abs(height_delta));
        if (linear_velocity < 3.0f) { linear_velocity_input += 3.0f - linear_velocity; linear_velocity = 3.0f; }
        Vector3 fwd = dragon_rb->get_global_transform().basis.get_column(0);
        dragon_rb->set_linear_velocity(fwd * linear_velocity);
        self->handleAngular(dragon_rb);
    }
};

class DragonControlTop : public Node {
    GDCLASS(DragonControlTop, Node);
public:
    // 子类须实现：提供线性速度输入与角度处理
    virtual float getLinearInput() = 0;
    virtual void handleAngular(RigidBody3D *rb) = 0;
    DragonControlTop();
    ~DragonControlTop();
    void _physics_process(double delta) override;
protected:
    static void _bind_methods();
private:
    DragonControlLogic<DragonControlTop> logic;
};

}
#endif // DRAGON_CONTROL_TOP_H