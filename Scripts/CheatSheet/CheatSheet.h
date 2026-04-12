#ifndef CHEAT_SHEET_H
#define CHEAT_SHEET_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>

namespace godot
{
    enum CheatSheetState 
    {
        STATE_ATTACHED, 
        STATE_DETATCHED,
        STATE_HELD,
        STATE_MOUTHED,
        STATE_HELD_CRISIS,
        STATE_DISCARDED,
        STATE_COUNT1
    };

    class CheatSheet : public Node 
    {
        GDCLASS(CheatSheet, Node);

        public:
            CheatSheet();
            ~CheatSheet();
            void _ready() override;
            void _physics_process(double delta) override;
            void Detatch();
            void _on_pickable_picked_up(Node* pickable);
            void _on_pickable_dropped(Node* pickable);

        protected:
            static void _bind_methods();

        private:
            void SetupPickable();
            Ref<ShaderMaterial> material;
            MeshInstance3D* mesh = nullptr;
            RigidBody3D* pickable = nullptr;
            RigidBody3D* dragon = nullptr;
            float flutter_speed = 0.03f;
            // XRToolsPickable to set its local position every frame to keep it at the same global position
            // the setting of detatch_position is used to counteract this behavior
            Vector3 detatch_position = Vector3(0.934f - 0.45f, 0.315f - 0.65f, 0);
            Vector3 detatch_rotation = Vector3(0, 0, 0);
            Vector3 detatch_direction = Vector3(0, 0, 0);
            using StateProcessFunc = void (CheatSheet::*)(double);
            StateProcessFunc state_process_funcs[(int)CheatSheetState::STATE_COUNT1];
            CheatSheetState state_current;
            int delete_count = 0;
            void ProcessAttached(double delta);
            void ProcessDetatched(double delta);
            void ProcessHeld(double delta);
            void ProcessMouthed(double delta);
            void ProcessHeldCrisis(double delta);
            void ProcessDiscarded(double delta);
    };
}

VARIANT_ENUM_CAST(godot::CheatSheetState);

#endif // CHEAT_SHEET_H