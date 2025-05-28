#ifndef CHEAT_SHEET_H
#define CHEAT_SHEET_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>

using namespace godot;

class CheatSheet : public Node3D 
{
    GDCLASS(CheatSheet, Node3D);

    private:
        Ref<ShaderMaterial> material;

    protected:
        static void _bind_methods();

    public:
        CheatSheet();
        ~CheatSheet();

        void _ready() override;
        void _physics_process(double delta) override;
};
#endif // CHEAT_SHEET_H