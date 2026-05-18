#ifndef CONTROL_SCENE_TOP_H
#define CONTROL_SCENE_TOP_H

// =============================================================================
// Control_Scene_Top.h
//
// Menu_Esc is lazily initialised inside this class on the first ESC press.
// Non-home subclasses require zero changes - the original ShowEscapeMenu /
// HideEscapeMenu / _input_top interface is preserved exactly.
// =============================================================================

#include "Dragon_Pilot_Top.h"
#include "Control_Camera.h"
#include "Control_Main.h"
#include "Menu_Esc.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/input_event.hpp>

namespace godot
{
    class Control_Scene_Top : public Node
    {
    public:
        Control_Scene_Top()  = default;
        ~Control_Scene_Top() override = default;

        void _ready();
        void _process_top(double delta);
        void ReturnHome();
        void _input_top(const Ref<InputEvent>& event);

        bool is_home_scene = false;
        void ShowEscapeMenu();
        void HideEscapeMenu();
        void SetHomeScene(bool is_home);

    protected:
        static void _bind_methods();
        void SyncSkyTime(Node* time_of_day);

        Dragon_Pilot_Top*   dragon_pilot    = nullptr;
        Control_Camera*     ctrl_camera     = nullptr;
        Control_Main*       control_main    = nullptr;
        Node3D*             camera_main     = nullptr;

        bool escape_menu_visible    = false;
        bool esc_was_pressed        = false;

    private:
        Menu_Esc   escape_overlay;
        bool            overlay_initialized = false;

        // Mirrors the original SubViewportContainer search logic from ShowEscapeMenu.
        // Tries "../SubViewportContainer" then "../../../SubViewportContainer",
        // resolves the UI root, builds the json_fn from the Settings singleton,
        // and calls escape_overlay.initialize(). Idempotent after first success.
        bool _ensure_overlay_initialized();
    };

} // namespace godot

#endif // CONTROL_SCENE_TOP_H
