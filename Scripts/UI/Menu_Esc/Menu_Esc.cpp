// =============================================================================
// Menu_Esc.cpp
//
// ESC menu: builds two cards (Return / Continue) and routes the
// player's choice back to Control_Scene_Top via callbacks.
// =============================================================================

#include "Menu_Esc.h"

using namespace godot;

// =============================================================================
// initialize
// =============================================================================

void Menu_Esc::initialize(
    Control*            ui_root,
    std::function<String(const String&, const String&)> json_fn,
    VoidCallback        on_return,
    VoidCallback        on_continue)
{
    ui_root_ref   = ui_root;
    get_json_text = std::move(json_fn);
    cb_return     = std::move(on_return);
    cb_continue   = std::move(on_continue);

    // No Settings pointer needed - ESC cards carry no setting values
    cards.init(
        ui_root_ref,
        /*settings*/ nullptr,
        get_json_text,
        [this](int index, const CardData& data)
        {
            _on_card_used(index, data);
        });
}

// =============================================================================
// show / hide
// =============================================================================

void Menu_Esc::show()
{
    if (!ui_root_ref) return;
    cards.destroy();

    // Return card
    {
        CardData ret;
        ret.type         = CARD_ESCAPE_RETURN;
        ret.prop_name    = "return";
        ret.display_name = get_json_text("header_return", "Return");
        ret.value_text   = ret.display_name;
        cards.push_card(ret);
    }

    // Continue card
    {
        CardData cont;
        cont.type         = CARD_ESCAPE_CONTINUE;
        cont.prop_name    = "continue";
        cont.display_name = get_json_text("header_continue", "Continue");
        cont.value_text   = cont.display_name;
        cards.push_card(cont);
    }

    cards.layout();
    visible_flag = true;
}

void Menu_Esc::hide()
{
    cards.destroy();
    visible_flag = false;
}

// =============================================================================
// process / handle_input
// =============================================================================

void Menu_Esc::process(double delta)
{
    if (visible_flag)
        cards.animate(delta);
}

void Menu_Esc::handle_input(const Ref<InputEvent>& event)
{
    if (visible_flag)
        cards.handle_input(event);
}

// =============================================================================
// _on_card_used
// =============================================================================

void Menu_Esc::_on_card_used(int /*index*/, const CardData& data)
{
    if (data.type == CARD_ESCAPE_RETURN)
    {
        hide();
        if (cb_return) cb_return();
    }
    else if (data.type == CARD_ESCAPE_CONTINUE)
    {
        hide();
        if (cb_continue) cb_continue();
    }
}
