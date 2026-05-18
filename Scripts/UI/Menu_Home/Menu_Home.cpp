// =============================================================================
// Menu_Home.cpp
//
// Menu_Home: main menu + settings cards state machine.
//
// Low-level card node construction / layout / animation / drag -> Cards.cpp
// ESC overlay                                                  -> Menu_Esc.cpp
// =============================================================================

#include "Menu_Home.h"
#include "Settings.h"
#include "Control_Main.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// =============================================================================
// Constructor / destructor
// =============================================================================

Menu_Home::Menu_Home()  {}
Menu_Home::~Menu_Home() {}

// =============================================================================
// _bind_methods
// =============================================================================

void Menu_Home::_bind_methods() {}

// =============================================================================
// Initialize
// =============================================================================

void Menu_Home::Initialize(
    Node*           owner,
    Settings*       settings_ptr,
    Control_Main*   cm,
    Node*           viewport_container)
{
    if (owner && get_parent() != owner)
        owner->add_child(this);

    set_process(true);
    set_process_input(true);

    settings     = settings_ptr;
    control_main = cm;

    if (!viewport_container)
    {
        UtilityFunctions::printerr("Menu_Home: viewport_container is null");
        return;
    }

    Node* ui_root_node = viewport_container->get_node_or_null(
        NodePath("Viewport/CanvasLayer/Control"));
    ui_root = Object::cast_to<Control>(ui_root_node);
    if (!ui_root)
    {
        UtilityFunctions::printerr(
            "Menu_Home: Could not find UI root at Viewport/CanvasLayer/Control");
        return;
    }

    settings_panel = ui_root->get_node_or_null(NodePath("Settings_Panel"));
    if (!settings_panel)
        UtilityFunctions::printerr("Menu_Home: Could not find Settings_Panel under UI root");

    cards.init(
        ui_root,
        settings,
        [this](const String& key, const String& fallback) -> String
        {
            return _get_json_text(key, fallback);
        },
        [this](int index, const CardData& data)
        {
            _on_card_used(index, data);
        });

    _rebuild_menu_cards();
}

// =============================================================================
// _process / _input
// =============================================================================

void Menu_Home::_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint()) return;
    cards.animate(delta);
}

void Menu_Home::_input(const Ref<InputEvent>& event)
{
    if (Engine::get_singleton()->is_editor_hint()) return;
    if (cards.card_count() == 0) return;

    bool settings_visible =
        settings_panel && static_cast<bool>(settings_panel->get("visible"));

    // Only consume input when the visible panel matches the current state
    if ((hand_state == HAND_SETTINGS_MAIN || hand_state == HAND_SETTINGS_SUB)
        && !settings_visible)
        return;
    if (hand_state == HAND_MENU && settings_visible)
        return;

    cards.handle_input(event);
}

// =============================================================================
// Hand rebuild helpers
// =============================================================================

void Menu_Home::_rebuild_menu_cards()
{
    cards.destroy();
    hand_state      = HAND_MENU;
    sub_parent_prop = "";

    cards.set_host(ui_root);

    struct Menu_HomeEntry
    {
        const char* id;
        const char* key;
        const char* fallback;
        const char* scene;
        bool opens_settings;
        bool shows_badge;
    };

    const Menu_HomeEntry entries[] = {
        { "Menu_Settings", "header_settings", "Settings",        "",               true,  false },
        { "Menu_Tutorial", "header_tutorial", "Note Book",       "Scene_Tutorial", false, false },
        { "Menu_Practice", "header_practice", "Flight Practice", "Scene_Practice", false, false },
        { "Menu_TD",       "header_td",       "Test Drive",      "Scene_TD",       false, true  },
        { "Menu_Dodge",    "header_dodge",    "Dodge",           "Scene_Dodge",    false, false },
    };

    for (const Menu_HomeEntry& e : entries)
    {
        CardData data;
        data.type           = CARD_MENU;
        data.prop_name      = e.id;
        String member       = String(e.key);
        if (member.begins_with("header_"))
            member = member.substr(7);
        data.member_name    = member;
        data.display_name   = _get_json_text(e.key, e.fallback);
        data.value_text     = data.display_name;
        data.scene_name     = e.scene;
        data.opens_settings = e.opens_settings;
        data.shows_badge    = e.shows_badge;

        cards.push_card(data);
    }

    cards.layout();
    _update_badge_display();
}

void Menu_Home::_rebuild_settings_main_cards()
{
    cards.destroy();
    hand_state      = HAND_SETTINGS_MAIN;
    sub_parent_prop = "";

    if (!settings || !settings_panel) return;
    cards.set_host(Object::cast_to<Control>(settings_panel));

    current_settings_list = settings->GetExposedSettings();

    // Back card (leftmost)
    {
        CardData back;
        back.type         = CARD_BACK;
        back.display_name = _get_json_text("header_back", "Back");
        back.value_text   = back.display_name;
        cards.push_card(back);
    }

    for (int i = 0; i < current_settings_list.size(); i++)
    {
        Dictionary entry   = current_settings_list[i];
        String prop_name   = String(entry.get("prop_name", ""));
        String member_name = String(entry.get("member",    ""));
        if (prop_name.is_empty()) continue;

        CardData data;
        data.type         = CARD_SETTING;
        data.prop_name    = prop_name;
        data.member_name  = member_name;
        String header_key = member_name.is_empty() ? prop_name : member_name;
        data.display_name = _get_json_text("header_" + header_key, prop_name);
        data.variant_type = static_cast<int>(entry.get("variant_type", Variant::NIL));
        data.hint         = static_cast<int>(entry.get("hint",         PROPERTY_HINT_NONE));
        data.hint_string  = String(entry.get("hint_string",            ""));
        data.is_custom    = static_cast<bool>(entry.get("is_custom",   false));

        cards.push_card(data);
    }

    // Populate value labels from Settings before layout
    cards.refresh_value_labels();
    cards.layout();
}

void Menu_Home::_rebuild_sub_cards(
    const String& prop_name, const CardData& parent_data)
{
    cards.destroy();
    hand_state      = HAND_SETTINGS_SUB;
    sub_parent_prop = prop_name;

    // Back card inherits parent metadata so we can rebuild correctly on return
    {
        CardData back;
        back.type         = CARD_BACK;
        back.prop_name    = prop_name;
        back.member_name  = parent_data.member_name;
        back.display_name = _get_json_text("header_back", "Back");
        back.value_text   = back.display_name;
        back.variant_type = parent_data.variant_type;
        back.hint         = parent_data.hint;
        back.hint_string  = parent_data.hint_string;
        back.is_custom    = parent_data.is_custom;
        cards.push_card(back);
    }

    String header_key  = parent_data.member_name.is_empty()
        ? prop_name : parent_data.member_name;
    String sub_display = _get_json_text("header_" + header_key, prop_name);

    if (parent_data.hint == PROPERTY_HINT_ENUM)
    {
        PackedStringArray items = parent_data.hint_string.split(",", false);
        for (int idx = 0; idx < items.size(); idx++)
        {
            CardData opt;
            opt.type         = CARD_OPTION;
            opt.prop_name    = prop_name;
            opt.member_name  = parent_data.member_name;
            opt.display_name = sub_display;
            opt.value_text   = items[idx].strip_edges();
            opt.variant_type = parent_data.variant_type;
            opt.hint         = parent_data.hint;
            opt.hint_string  = parent_data.hint_string;
            opt.is_custom    = parent_data.is_custom;
            opt.option_value = idx;
            cards.push_card(opt);
        }
    }
    else if (parent_data.hint == PROPERTY_HINT_RANGE)
    {
        PackedStringArray parts = parent_data.hint_string.split(",", false);
        int range_min = 0, range_max = 3;
        if (parts.size() >= 2)
        {
            range_min = static_cast<int>(parts[0].strip_edges().to_int());
            range_max = static_cast<int>(parts[1].strip_edges().to_int());
        }
        for (int v = range_min; v <= range_max; v++)
        {
            CardData opt;
            opt.type         = CARD_OPTION;
            opt.prop_name    = prop_name;
            opt.member_name  = parent_data.member_name;
            opt.display_name = sub_display;
            opt.value_text   = String::num_int64(v);
            opt.variant_type = parent_data.variant_type;
            opt.hint         = parent_data.hint;
            opt.hint_string  = parent_data.hint_string;
            opt.is_custom    = parent_data.is_custom;
            opt.option_value = v;
            cards.push_card(opt);
        }
    }

    cards.layout();
}

// =============================================================================
// Card event dispatch
// =============================================================================

void Menu_Home::_on_card_used(int /*index*/, const CardData& data)
{
    if (data.type == CARD_MENU)
    {
        if (data.opens_settings)
            _on_settings_button_pressed();
        else if (!data.scene_name.is_empty())
            _on_button_pressed(data.scene_name);
        return;
    }

    if (data.type == CARD_BACK)
    {
        if (hand_state == HAND_SETTINGS_MAIN)
        {
            // Closing the settings panel returns to the main menu
            if (settings_panel)
                settings_panel->set("visible", false);
            _on_close_button_pressed();
        }
        else
        {
            // Inside a sub-hand: go back to the settings main hand
            _rebuild_settings_main_cards();
        }
        return;
    }

    if (data.type == CARD_OPTION)
    {
        _apply_option_select(data);
        _rebuild_settings_main_cards();
        return;
    }

    // CARD_SETTING
    if (data.variant_type == Variant::BOOL)
    {
        // Toggle immediately; no sub-hand needed
        _apply_bool_toggle(data);
        cards.refresh_value_labels();
    }
    else
    {
        _rebuild_sub_cards(data.prop_name, data);
    }
}

void Menu_Home::_apply_bool_toggle(const CardData& data)
{
    if (!settings) return;
    bool current =
        static_cast<bool>(settings->call(data.prop_name + String("_getter")));
    settings->call(data.prop_name + String("_setter"), !current);

    if (data.prop_name == "Language")
        _on_language_changed();
}

void Menu_Home::_apply_option_select(const CardData& data)
{
    if (!settings) return;
    settings->call(data.prop_name + String("_setter"), data.option_value);

    if (data.prop_name == "Language")
        _on_language_changed();
}

// =============================================================================
// Scene / panel transitions
// =============================================================================

void Menu_Home::_on_button_pressed(const String& scene_name)
{
    if (control_main)
        control_main->Switch_Scene(scene_name);
    else
        UtilityFunctions::printerr("Menu_Home: Control_Main not available for scene switch");
}

void Menu_Home::_on_settings_button_pressed()
{
    if (!settings_panel) return;
    settings_panel->set("visible", true);
    cards.set_host(Object::cast_to<Control>(settings_panel));
    _rebuild_settings_main_cards();
}

void Menu_Home::_on_close_button_pressed()
{
    if (settings_panel)
        settings_panel->set("visible", false);
    cards.set_host(ui_root);
    _rebuild_menu_cards();
}

// =============================================================================
// Helpers
// =============================================================================

void Menu_Home::_on_language_changed()
{
    if (hand_state == HAND_MENU)
    {
        _rebuild_menu_cards();
    }
    else if (hand_state == HAND_SETTINGS_MAIN)
    {
        _rebuild_settings_main_cards();
    }
    else if (hand_state == HAND_SETTINGS_SUB)
    {
        // Rebuild the sub-hand using the cached settings list entry
        for (int i = 0; i < current_settings_list.size(); i++)
        {
            Dictionary entry = current_settings_list[i];
            if (String(entry.get("prop_name", "")) != sub_parent_prop) continue;

            CardData parent;
            parent.prop_name    = sub_parent_prop;
            parent.member_name  = String(entry.get("member",       ""));
            parent.variant_type = static_cast<int>(
                entry.get("variant_type", Variant::NIL));
            parent.hint         = static_cast<int>(
                entry.get("hint",         PROPERTY_HINT_NONE));
            parent.hint_string  = String(entry.get("hint_string",  ""));
            parent.is_custom    = static_cast<bool>(
                entry.get("is_custom",    false));

            _rebuild_sub_cards(sub_parent_prop, parent);
            return;
        }
    }
}

void Menu_Home::_update_badge_display()
{
    Node* badge_node = cards.badge_icon;
    if (!badge_node || !settings) return;

    TextureRect* texture_rect = Object::cast_to<TextureRect>(badge_node);
    if (!texture_rect) return;

    int badge_value = settings->GetValBadge();
    if (badge_value == 0)
    {
        texture_rect->set_texture(Ref<Texture2D>());
        texture_rect->set_visible(false);
        return;
    }

    String texture_path;
    switch (badge_value)
    {
        case 1: texture_path = "res://Media/Image/badge_1.png"; break;
        case 2: texture_path = "res://Media/Image/badge_2.png"; break;
        case 3: texture_path = "res://Media/Image/badge_3.png"; break;
        default:
            UtilityFunctions::printerr("Menu_Home: Invalid badge value: ", badge_value);
            return;
    }

    Ref<Texture2D> texture = ResourceLoader::get_singleton()->load(texture_path);
    if (texture.is_valid())
    {
        texture_rect->set_texture(texture);
        texture_rect->set_visible(true);
    }
    else
    {
        UtilityFunctions::printerr("Menu_Home: Failed to load badge texture: ", texture_path);
        texture_rect->set_visible(false);
    }
}

String Menu_Home::_get_json_text(const String& key, const String& fallback)
{
    if (!settings) return fallback;

    // Cache the parsed dictionary so we don't re-open and re-parse the JSON
    // file on every single card push_card() call.  The cache is invalidated
    // when the language value changes.
    static int    cached_lang = -1;
    static Dictionary cached_dict;

    int lang = settings->GetValLanguage();
    if (lang != cached_lang)
    {
        cached_lang = lang;
        cached_dict = Dictionary();   // clear stale cache

        String json_file = (lang == 1)
            ? "res://Media/Text/Chinese.json"
            : "res://Media/Text/English.json";

        Ref<FileAccess> f = FileAccess::open(json_file, FileAccess::READ);
        if (f.is_valid())
        {
            String content = f->get_as_text();
            f->close();

            Ref<JSON> json = memnew(JSON);
            if (json->parse(content) == OK)
                cached_dict = json->get_data();
        }
    }

    if (cached_dict.has(key))
        return String(cached_dict[key]);

    return fallback;
}
