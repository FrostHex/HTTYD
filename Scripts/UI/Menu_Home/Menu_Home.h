#ifndef MENU_HOME_H
#define MENU_HOME_H

// =============================================================================
// Menu_Home.h
//
// Menu_Home: state machine for the main menu cards and the settings cards (GDClass Node).
//
// Hand states:
//   HAND_MENU          - shows the main menu cards
//   HAND_SETTINGS_MAIN - shows all exposed settings as cards, fan layout
//   HAND_SETTINGS_SUB  - shows the concrete options for one enum/int setting
//
// The ESC overlay has been moved to Menu_Esc and is managed directly by
// Control_Scene_Top; Menu_Home no longer handles ESC at all.
// =============================================================================

#include "Cards.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class Settings;
    class Control_Main;
    class Control;
    class InputEvent;

    class Menu_Home : public Node
    {
        GDCLASS(Menu_Home, Node);

    public:
        Menu_Home();
        ~Menu_Home();

        // Call from the owning scene's _ready() to wire up all references.
        void Initialize(
            Node*           owner,
            Settings*       settings,
            Control_Main*   control_main,
            Node*           viewport_container);

        void _process(double delta) override;
        void _input(const Ref<InputEvent>& event) override;

    protected:
        static void _bind_methods();

    private:
        // -----------------------------------------------------------------
        // Hand state machine
        // -----------------------------------------------------------------

        enum HandState
        {
            HAND_MENU,
            HAND_SETTINGS_MAIN,
            HAND_SETTINGS_SUB,
        };

        HandState   hand_state       = HAND_MENU;
        String      sub_parent_prop; // prop_name of the currently expanded setting

        // -----------------------------------------------------------------
        // Core references
        // -----------------------------------------------------------------

        Settings*       settings        = nullptr;
        Control_Main*   control_main    = nullptr;
        Control*        ui_root         = nullptr;
        Node*           settings_panel  = nullptr;

        Array   current_settings_list;  // Cached result of GetExposedSettings()

        // -----------------------------------------------------------------
        // Cards manager
        // -----------------------------------------------------------------

        Cards cards;

        // -----------------------------------------------------------------
        // Hand rebuild helpers
        // -----------------------------------------------------------------

        void _rebuild_menu_cards();
        void _rebuild_settings_main_cards();
        void _rebuild_sub_cards(const String& prop_name, const CardData& parent_data);

        // -----------------------------------------------------------------
        // Card event dispatch
        // -----------------------------------------------------------------

        void _on_card_used(int index, const CardData& data);
        void _apply_bool_toggle(const CardData& data);
        void _apply_option_select(const CardData& data);

        // -----------------------------------------------------------------
        // Scene / panel transitions
        // -----------------------------------------------------------------

        void _on_button_pressed(const String& scene_name);
        void _on_settings_button_pressed();
        void _on_close_button_pressed();

        // -----------------------------------------------------------------
        // Helpers
        // -----------------------------------------------------------------

        void   _update_badge_display();
        void   _on_language_changed();
        String _get_json_text(const String& key, const String& fallback = "");
    };

} // namespace godot

#endif // MENU_HOME_H
