#ifndef CARDS_H
#define CARDS_H

// =============================================================================
// Cards.h
//
// Card hand system: data structures, node construction, fan layout,
// per-frame animation, and drag/hover interaction.
//
// Cards is a plain C++ utility class (not a GDClass).
// It is owned by Menu_Home and Menu_Esc and is responsible for:
//   - Creating and destroying card Control nodes under a given host
//   - Fan layout calculation
//   - Per-frame interpolation animation
//   - Mouse hover and drag detection, reporting results via on_card_used
// =============================================================================

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string.hpp>
#include <functional>

namespace godot
{
    class Settings;

    // -------------------------------------------------------------------------
    // CardType
    // -------------------------------------------------------------------------

    enum CardType
    {
        CARD_MENU,              // Main menu card
        CARD_SETTING,           // Settings hand: represents one setting entry
        CARD_OPTION,            // Sub-hand: one concrete enum/int value
        CARD_BACK,              // Sub-hand: go back to the parent hand
        CARD_ESCAPE_RETURN,     // ESC overlay: return to home scene
        CARD_ESCAPE_CONTINUE,   // ESC overlay: continue playing
    };

    // -------------------------------------------------------------------------
    // CardData  (POD-like, safe to copy)
    // -------------------------------------------------------------------------

    struct CardData
    {
        CardType    type            = CARD_MENU;
        String      prop_name;          // Corresponding Settings property name
        String      member_name;        // Settings member in snake_case (JSON key)
        String      display_name;       // Text shown at the top of the card
        String      value_text;         // Text shown in the centre of the card
        int         option_value   = 0; // CARD_OPTION only: the int value this option represents
        int         variant_type   = 0; // Variant::Type of the setting
        int         hint           = 0; // PropertyHint of the setting
        String      hint_string;
        bool        is_custom      = false;
        String      scene_name;         // CARD_MENU only: target scene name
        bool        opens_settings = false; // CARD_MENU: opens the settings panel
        bool        shows_badge    = false; // CARD_MENU: shows the badge icon overlay
    };

    // -------------------------------------------------------------------------
    // CardNode  (holds live scene-tree node pointers for one card)
    // -------------------------------------------------------------------------

    struct CardNode
    {
        CardData     data;
        Control*     root_ctrl    = nullptr;  // Root Control node of the card
        Panel*       panel        = nullptr;  // Transparent panel (kept for compatibility)
        Label*       title_label  = nullptr;  // Title label at the top
        Label*       value_label  = nullptr;  // Central value label
        TextureRect* value_image  = nullptr;  // Central value image (overrides label when set)
        TextureRect* widget_image = nullptr;  // Bool toggle widget image
        Label*       detail_label = nullptr;  // Detail/description label at the bottom

        // Animation targets - computed by Cards::layout(), lerped by animate()
        Vector2      target_pos;
        float        target_rot   = 0.f;
        Vector2      target_scale = Vector2(1, 1);
        float        target_z     = 0.f;
    };

    // =========================================================================
    // Cards - hand manager
    // =========================================================================

    class Cards
    {
    public:
        // Callback invoked when the player releases a card above the threshold
        using UsedCallback = std::function<void(int index, const CardData&)>;

        // ---------------------------------------------------------------------
        // Layout constants
        // ---------------------------------------------------------------------

        static constexpr float CARD_WIDTH           = 180.f;
        static constexpr float CARD_HEIGHT          = 250.f;
        static constexpr float FAN_RADIUS           = 2000.f;   // Virtual circle radius for the fan
        static constexpr float FAN_MAX_ANGLE_DEG    = 18.5f;    // Total fan spread in degrees
        static constexpr float HOVER_SCALE          = 2.f;
        static constexpr float HOVER_LIFT           = 30.f;     // Pixels lifted when hovered
        static constexpr float ANIM_SPEED           = 35.f;     // Lerp speed multiplier
        static constexpr float DRAG_RELEASE_HEIGHT  = 80.f;     // Min upward drag to trigger use

        // Internal card region layout constants ---

        // Type badge (top-right corner)
        static constexpr float TYPE_LABEL_X         = 148.f;
        static constexpr float TYPE_LABEL_Y         = 9.f;
        static constexpr float TYPE_LABEL_W         = 22.f;
        static constexpr float TYPE_LABEL_H         = 22.f;
        static constexpr int   TYPE_LABEL_FONT_SIZE = 18;

        // Title region
        static constexpr float TITLE_X              = 40.f;
        static constexpr float TITLE_Y              = 11.f;
        static constexpr float TITLE_W_MARGIN       = 79.f;     // Width = CARD_WIDTH - margin
        static constexpr float TITLE_H              = 23.f;
        static constexpr int   TITLE_FONT_SIZE      = 14;

        // Central value region
        static constexpr float VALUE_X              = 10.f;
        static constexpr float VALUE_Y_RATIO        = 0.15f;    // Relative to CARD_HEIGHT
        static constexpr float VALUE_H_RATIO        = 0.54f;    // Relative to CARD_HEIGHT
        static constexpr float VALUE_W_MARGIN       = 19.f;
        static constexpr int   VALUE_FONT_SIZE      = 17;
        static constexpr float VALUE_IMAGE_SCALE    = 0.95f;

        // Bool widget image
        static constexpr float WIDGET_X             = 20.f;
        static constexpr float WIDGET_Y_RATIO       = 0.33f;    // Relative to CARD_HEIGHT
        static constexpr float WIDGET_W             = 90.f;
        static constexpr float WIDGET_H             = 90.f;
        static constexpr float WIDGET_IMAGE_SCALE   = 1.0f;

        // Detail / description region
        static constexpr float DETAIL_X             = 10.f;
        static constexpr float DETAIL_Y_RATIO       = 0.805f;   // Relative to CARD_HEIGHT
        static constexpr float DETAIL_H_RATIO       = 0.16f;    // Relative to CARD_HEIGHT
        static constexpr float DETAIL_W_MARGIN      = 20.f;
        static constexpr int   DETAIL_FONT_SIZE     = 9;
        static constexpr int   DETAIL_LINE_SPACING  = 0;

        // ---------------------------------------------------------------------
        // Public interface
        // ---------------------------------------------------------------------

        // Must be called once before any other method.
        //   host         : the Control under which the card root will be parented
        //   settings     : used to read setting values; may be nullptr for ESC-only hands
        //   json_fn      : localization callback  (key, fallback) -> String
        //   used_callback: fired when the player drags a card past the release threshold
        //   debug_ui     : when true, coloured debug overlays are drawn on each card region
        void init(
            Control*            host,
            Settings*           settings,
            std::function<String(const String&, const String&)> json_fn,
            UsedCallback        used_callback,
            bool                debug_ui = false);

        // Reparent the card root to a different host Control
        void set_host(Control* host);

        // Add one card to the hand (call before layout())
        void push_card(const CardData& data);

        // Compute target_* fields for every card in the current hand
        void layout();

        // Lerp all cards toward their target state; call every frame from _process()
        void animate(double delta);

        // Free all card nodes and reset state
        void destroy();

        // Forward mouse events from _input(); returns true if the event was consumed
        bool handle_input(const Ref<InputEvent>& event);

        // Re-read current setting values from Settings and update all value labels
        void refresh_value_labels();

        // Refresh the value display of a single card node
        void update_value_display(CardNode& cn);

        // Badge icon node - set by _create_card() when CardData::shows_badge is true
        Node* badge_icon = nullptr;

        int                      card_count()   const { return hand_cards.size(); }
        const Vector<CardNode>&  cards()        const { return hand_cards; }
        Vector<CardNode>&        cards_write()        { return hand_cards; }

    private:
        Control*    host_node       = nullptr;
        Control*    cards_root  = nullptr;
        Settings*   settings_ref    = nullptr;
        bool        debug_ui_flag   = false;

        std::function<String(const String&, const String&)> get_json_text;
        UsedCallback on_card_used_cb;

        Vector<CardNode> hand_cards;

        int     hovered_index   = -1;
        int     dragged_index   = -1;
        Vector2 drag_start_pos;
        Vector2 drag_card_start;
        bool    drag_triggered  = false;

        Color dark_gray = Color(0.12f, 0.12f, 0.12f, 1.f);

        void        _build_root();
        CardNode    _create_card(const CardData& data, int index);
        Vector2     _local_mouse_pos() const;

        String      _get_value_image_path(const CardData& data) const;
        String      _format_setting_value(const CardData& data) const;
    };

} // namespace godot

#endif // CARDS_H
