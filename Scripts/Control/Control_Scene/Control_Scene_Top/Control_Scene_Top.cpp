// =============================================================================
// Control_Scene_Top.cpp
//
// Base scene controller shared by all non-trivial game scenes.
// Owns Menu_Esc and initialises it lazily on the first ESC press so that
// non-home subclasses need zero modifications.
// =============================================================================

#include "Control_Scene_Top.h"

#include "Control_Camera.h"
#include "Settings.h"

#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// =============================================================================
// _ready
// =============================================================================

void Control_Scene_Top::_ready()
{
    camera_main = get_tree()->get_root()->get_node<Node3D>("Main/Camera_Main");
}

// =============================================================================
// _process - drive Menu_Esc animation every frame
// =============================================================================

void Control_Scene_Top::_process_top(double delta)
{
    if (escape_menu_visible)
        escape_overlay.process(delta);
}

// =============================================================================
// SetHomeScene
// =============================================================================

void Control_Scene_Top::SetHomeScene(bool is_home)
{
    is_home_scene = is_home;
}

// =============================================================================
// _ensure_overlay_initialized
//
// Equivalent to the original SubViewportContainer lookup that was inline in
// ShowEscapeMenu: first tries "../SubViewportContainer", then falls back to
// "../../../SubViewportContainer".  On success, resolves the UI root Control,
// constructs a json_fn from the Settings singleton (matching Menu's own text
// lookup), and calls escape_overlay.initialize() once.
// =============================================================================

bool Control_Scene_Top::_ensure_overlay_initialized()
{
    if (overlay_initialized) return true;

    // --- Locate SubViewportContainer (same two paths as the original code) ---
    Node* vp = get_node_or_null(NodePath("../SubViewportContainer"));
    if (!vp)
        vp = get_node_or_null(NodePath("../../../SubViewportContainer"));

    if (!vp)
    {
        UtilityFunctions::printerr(
            "Control_Scene_Top: Could not find SubViewportContainer "
            "for Menu_Esc");
        return false;
    }

    // --- Resolve UI root ---
    Node* ui_root_node =
        vp->get_node_or_null(NodePath("Viewport/CanvasLayer/Control"));
    Control* ui_root = Object::cast_to<Control>(ui_root_node);

    if (!ui_root)
    {
        UtilityFunctions::printerr(
            "Control_Scene_Top: Could not find UI root at "
            "Viewport/CanvasLayer/Control");
        return false;
    }

    // --- Build json_fn using the Settings singleton (matches Menu::_get_json_text) ---
    Settings* settings = Settings::GetSingleton();

    auto json_fn = [settings](
        const String& key, const String& fallback) -> String
    {
        if (!settings) return fallback;

        String json_file = "res://Media/Text/English.json";
        switch (settings->GetValLanguage())
        {
            case LANGUAGE_CHINESE:
                json_file = "res://Media/Text/Chinese.json";
                break;
            case LANGUAGE_RUNIC:
                json_file = "res://Media/Text/Runic.json";
                break;
            default:
                break;
        }

        Ref<FileAccess> f = FileAccess::open(json_file, FileAccess::READ);
        if (f.is_valid())
        {
            String content = f->get_as_text();
            f->close();
            Ref<JSON> json = memnew(JSON);
            if (json->parse(content) == OK)
            {
                Dictionary data = json->get_data();
                if (data.has(key))
                    return String(data[key]);
            }
        }
        return fallback;
    };

    // --- Initialize Menu_Esc ---
    escape_overlay.initialize(
        ui_root,
        settings,
        std::move(json_fn),
        // on_return: take the player back to Scene_Home
        [this]() { ReturnHome(); },
        // on_continue: dismiss overlay and restore mouse capture.
        // Mirrors the behaviour of the original HideEscapeMenu.
        [this]()
        {
            escape_menu_visible = false;
            esc_was_pressed     = false;
            Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
        });

    overlay_initialized = true;
    return true;
}

// =============================================================================
// ShowEscapeMenu
// =============================================================================

void Control_Scene_Top::ShowEscapeMenu()
{
    if (is_home_scene) return;

    if (!_ensure_overlay_initialized()) return;

    escape_overlay.show();
    escape_menu_visible = true;
}

// =============================================================================
// HideEscapeMenu
// =============================================================================

void Control_Scene_Top::HideEscapeMenu()
{
    escape_menu_visible = false;
    esc_was_pressed     = false;  // Force-reset state, matching original logic
    escape_overlay.hide();
}

// =============================================================================
// SyncSkyTime
// =============================================================================

void Control_Scene_Top::SyncSkyTime(Node* time_of_day)
{
    if (!time_of_day) return;

    Time* time_singleton = Time::get_singleton();
    if (!time_singleton)
    {
        UtilityFunctions::printerr(
            "Control_Scene_Top: Time singleton is unavailable");
        return;
    }

    Dictionary datetime_dict =
        time_singleton->get_datetime_dict_from_system(false);

    if (time_of_day->has_method("set_from_datetime_dict"))
    {
        time_of_day->call("set_from_datetime_dict", datetime_dict);
    }
    else
    {
        time_of_day->set("year",  datetime_dict["year"]);
        time_of_day->set("month", datetime_dict["month"]);
        time_of_day->set("day",   datetime_dict["day"]);

        if (time_of_day->has_method("set_time"))
        {
            time_of_day->call(
                "set_time",
                datetime_dict["hour"],
                datetime_dict["minute"],
                datetime_dict["second"]);
        }
    }
}

// =============================================================================
// _input_top - ESC key handling + mouse event forwarding to Menu_Esc
// =============================================================================

void Control_Scene_Top::_input_top(const Ref<InputEvent>& event)
{
    if (!event.is_valid()) return;

    // Forward all input to the overlay while it is visible
    if (escape_menu_visible)
        escape_overlay.handle_input(event);

    // Only care about key events from here on
    Ref<InputEventKey> key_event = event;
    if (key_event.is_null()) return;
    if (key_event->get_keycode() != Key::KEY_ESCAPE) return;

    bool esc_pressed = key_event->is_pressed();
    bool esc_echo    = key_event->is_echo();

    if (esc_echo) return;  // Ignore key-repeat events

    if (!esc_pressed)
    {
        esc_was_pressed = false;
        return;
    }
    if (esc_was_pressed) return;  // Already handled the initial press
    esc_was_pressed = true;

    if (is_home_scene) return;

    if (escape_menu_visible)
        HideEscapeMenu();
    else
    {
        Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
        ShowEscapeMenu();
    }
}

// =============================================================================
// ReturnHome
// =============================================================================

void Control_Scene_Top::ReturnHome()
{
    SceneTree* tree = get_tree();
    if (!tree)
    {
        UtilityFunctions::printerr("SceneTree not available");
        return;
    }

    Window* root = tree->get_root();
    if (!root)
    {
        UtilityFunctions::printerr("Root window not available");
        return;
    }

    set_physics_process(false);

    if (dragon_pilot && dragon_pilot->is_inside_tree())
        dragon_pilot->set_physics_process(false);

    if (ctrl_camera && ctrl_camera->is_inside_tree())
    {
        ctrl_camera->set_physics_process(false);
        ctrl_camera->set_process_input(false);
        ctrl_camera->SetFreeCamera(false);
    }

    // Restore camera_main to its original position under Main
    if (camera_main && camera_main->is_inside_tree() && root)
    {
        Node* main_node = root->get_node_or_null(NodePath("Main"));
        if (main_node)
        {
            camera_main->reparent(main_node);
            camera_main->call_deferred("set_position", Vector3(0.0f, 10.0f, 0.0f));
            camera_main->call_deferred("set_rotation", Vector3(0.0f, -200 * Math_PI / 180, 0.0f));
            
            Node* xr_origin =
                camera_main->get_node_or_null(NodePath("XR/XROrigin"));
            if (xr_origin)
            {
                xr_origin->call_deferred(
                    "set_position", Vector3(0.0f, 0.0f, 0.0f));

                Node* sub_viewport_mesh =
                    xr_origin->get_node_or_null(
                        NodePath("XRCamera/SubViewportMesh"));
                if (sub_viewport_mesh)
                    sub_viewport_mesh->queue_free();
            }
            // UtilityFunctions::print("Camera_Main restored to original position");
        }
        else
        {
            UtilityFunctions::printerr(
                "Could not find Main node to restore camera");
        }
    }
    else if (camera_main && !camera_main->is_inside_tree())
    {
        UtilityFunctions::printerr("camera_main is not in scene tree");
    }
    else
    {
        UtilityFunctions::printerr(
            "camera_main not found when returning to Scene_Home");
    }

    // Locate Control_Main if we do not already have a cached reference
    if (!control_main)
    {
        Node* cm = root->get_node_or_null(NodePath("Main/Control_Main"));
        if (!cm)
            cm = root->find_child("Control_Main", true, false);
        control_main = Object::cast_to<Control_Main>(cm);
    }

    if (control_main)
    {
        control_main->call("Switch_Scene", "Scene_Home");
    }
    else
    {
        UtilityFunctions::printerr(
            "Control_Scene_Top: Control_Main not available to switch scene. "
            "Run from the Main scene to enable navigation.");
    }

    Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
}

// =============================================================================
// _bind_methods
// =============================================================================

void Control_Scene_Top::_bind_methods()
{
    ClassDB::bind_method(
        D_METHOD("ReturnHome"),
        &Control_Scene_Top::ReturnHome);

    ClassDB::bind_method(
        D_METHOD("ShowEscapeMenu"),
        &Control_Scene_Top::ShowEscapeMenu);

    ClassDB::bind_method(
        D_METHOD("HideEscapeMenu"),
        &Control_Scene_Top::HideEscapeMenu);

    ClassDB::bind_method(
        D_METHOD("SetHomeScene", "is_home"),
        &Control_Scene_Top::SetHomeScene);
}
