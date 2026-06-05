#ifndef MENU_ESC_H
#define MENU_ESC_H

// =============================================================================
// Menu_Esc.h
//
// Lightweight ESC pause overlay that owns exactly two cards: Return and Continue.
//
// Responsibilities:
//   - Build and destroy the two ESC card nodes under a given ui_root
//   - Notify the caller via callbacks when the player chooses an action
//   - No dependency on Settings; no game state is held here
//
// Usage:
//   Menu_Esc overlay;
//   overlay.initialize(ui_root, json_fn, on_return, on_continue);
//   overlay.show();   // build and display the two cards
//   overlay.hide();   // destroy the card nodes
//
// Call process() every frame and handle_input() from _input() while visible.
// =============================================================================

#include "Cards.h"
#include "Settings.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <functional>

namespace godot
{
    class Menu_Esc
    {
    public:
        using VoidCallback = std::function<void()>;

        Menu_Esc()  = default;
        ~Menu_Esc() = default;

        // Must be called once before show() / hide().
        //   ui_root    : cards will be parented under this node
        //   settings   : required for language-aware font selection
        //   json_fn    : localization callback  (key, fallback) -> String
        //   on_return  : invoked when the player selects "Return to Home"
        //   on_continue: invoked when the player selects "Continue"
        void initialize(
            Control*            ui_root,
            Settings*           settings,
            std::function<String(const String&, const String&)> json_fn,
            VoidCallback        on_return,
            VoidCallback        on_continue);

        void show();                            // Build and display the two ESC cards
        void hide();                            // Destroy the card nodes
        bool is_visible() const { return visible_flag; }

        // Forward from _input() - only active while visible
        void handle_input(const Ref<InputEvent>& event);

        // Drive card animation - call every frame from _process()
        void process(double delta);

    private:
        Control*     ui_root_ref  = nullptr;
        bool         visible_flag = false;

        Settings*    settings_ref = nullptr;
        Cards        cards;

        std::function<String(const String&, const String&)> get_json_text;
        VoidCallback cb_return;
        VoidCallback cb_continue;

        void _on_card_used(int index, const CardData& data);
    };

} // namespace godot

#endif // MENU_ESC_H
