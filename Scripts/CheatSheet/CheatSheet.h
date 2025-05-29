#ifndef CHEAT_SHEET_H
#define CHEAT_SHEET_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>

using namespace godot;

class CheatSheet : public Node3D 
{
    GDCLASS(CheatSheet, Node3D);

    private:
        Ref<ShaderMaterial> material;
        MeshInstance3D* mesh = nullptr;
        RigidBody3D* pickable = nullptr;
        bool detatched = false;
        Vector3 detatch_direction = Vector3(0, 0, 0);

    protected:
        static void _bind_methods();

    public:
        CheatSheet();
        ~CheatSheet();
        void _ready() override;
        void _physics_process(double delta) override;
        void Detatch();
};
#endif // CHEAT_SHEET_H