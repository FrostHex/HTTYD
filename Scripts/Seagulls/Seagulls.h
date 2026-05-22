// ==================== Seagulls.h ====================
#ifndef SEAGULLS_H
#define SEAGULLS_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    class Seagulls : public Node3D
    {
        GDCLASS(Seagulls, Node3D);

    public:
        Seagulls();
        ~Seagulls();
        void _ready() override;

    private:
        static void _bind_methods();
        void initialize_seagulls();

        static constexpr float speed = 2.7f;
    };
}

#endif // SEAGULLS_H