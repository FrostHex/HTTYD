// =============================================================================
// Cards.cpp
//
// Cards implementation: node construction, fan layout, per-frame animation,
// drag/hover interaction, and value display helpers.
//
// Everything here is agnostic to which hand state is currently active;
// that logic lives in Menu_Home and Menu_Esc.
// =============================================================================

#include "Cards.h"
#include "Settings.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/system_font.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

using namespace godot;

// =============================================================================
// init / set_host
// =============================================================================

void Cards::init(
    Control*            host,
    Settings*           settings,
    std::function<String(const String&, const String&)> json_fn,
    UsedCallback        used_callback,
    bool                debug_ui)
{
    host_node        = host;
    settings_ref     = settings;
    get_json_text    = std::move(json_fn);
    on_card_used_cb  = std::move(used_callback);
    debug_ui_flag    = debug_ui;

    _build_root();
}

void Cards::set_host(Control* host)
{
    if (!cards_root || !host) return;
    if (cards_root->get_parent() == host) return;

    cards_root->reparent(host);
    host_node = host;

    // 确保手牌根节点水平居中（全宽 + 水平居中对齐）
    cards_root->set_anchor_and_offset(SIDE_LEFT,   0.0f, 0.f);
    cards_root->set_anchor_and_offset(SIDE_RIGHT,  1.0f, 0.f);
    cards_root->set_anchor_and_offset(SIDE_BOTTOM, 1.0f, -10.f);
    cards_root->set_anchor_and_offset(SIDE_TOP,    1.0f, -10.f);
    cards_root->set_size(Vector2(0, 0));  // size(0,0) + 左右锚点1.0/0.0 会自动撑满父容器宽度
}

// =============================================================================
// _build_root - create the anchored root Control that holds all cards
// =============================================================================

void Cards::_build_root()
{
    if (cards_root || !host_node) return;

    Control* root = memnew(Control);
    root->set_name("CardsRoot");
    
    // 关键修复：水平全宽 + 居中，保证扇形围绕屏幕/面板水平中心
    root->set_anchor_and_offset(SIDE_LEFT,   0.0f, 0.f);
    root->set_anchor_and_offset(SIDE_RIGHT,  1.0f, 0.f);
    root->set_anchor_and_offset(SIDE_BOTTOM, 1.0f, -10.f);
    root->set_anchor_and_offset(SIDE_TOP,    1.0f, -10.f);
    root->set_size(Vector2(0, 0));
    root->set_clip_contents(false);
    root->set_texture_filter(Control::TEXTURE_FILTER_NEAREST);

    host_node->add_child(root);
    cards_root = root;
}

// =============================================================================
// push_card / destroy
// =============================================================================

void Cards::push_card(const CardData& data)
{
    if (!cards_root) return;
    CardNode cn = _create_card(data, hand_cards.size());
    hand_cards.push_back(cn);
}

void Cards::destroy()
{
    for (int i = 0; i < hand_cards.size(); i++)
    {
        if (hand_cards[i].root_ctrl)
            hand_cards[i].root_ctrl->queue_free();
    }
    hand_cards.clear();
    hovered_index  = -1;
    dragged_index  = -1;
    drag_triggered = false;
    badge_icon     = nullptr;
}

// =============================================================================
// layout - compute fan target_* fields for every card
// =============================================================================

void Cards::layout()
{
    int n = hand_cards.size();
    if (n == 0) return;

    // Scale the total spread down proportionally for small hands
    float total_angle = FAN_MAX_ANGLE_DEG;
    if (n < 5)
        total_angle = FAN_MAX_ANGLE_DEG * (static_cast<float>(n) - 1.f) / 4.f;

    float step = (n > 1) ? total_angle / static_cast<float>(n - 1) : 0.f;

    // 获取根节点的实际宽度，用于水平居中（关键修复）
    float root_width = cards_root->get_size().x;
    if (root_width <= 0.f)
        root_width = cards_root->get_parent_area_size().x;  // fallback

    float center_x = root_width * 0.5f;

    for (int i = 0; i < n; i++)
    {
        CardNode& cn = hand_cards.write[i];

        float angle_deg = -total_angle * 0.5f + step * static_cast<float>(i);
        float angle_rad = Math::deg_to_rad(angle_deg);

        float x_offset = FAN_RADIUS * Math::sin(angle_rad);
        float y_offset = FAN_RADIUS * (1.f - Math::cos(angle_rad));

        Vector2 base_pos(
            center_x + x_offset - CARD_WIDTH * 0.5f,   // ← 加上 center_x 实现水平居中
            -300.f + y_offset);

        float   base_rot   = angle_deg;
        Vector2 base_scale(1.f, 1.f);
        float   base_z     = static_cast<float>(i);

        if (i == hovered_index && dragged_index < 0)
        {
            base_pos.y -= HOVER_LIFT;
            base_rot    = 0.f;
            base_scale  = Vector2(HOVER_SCALE, HOVER_SCALE);
            base_z      = static_cast<float>(n + 10);
        }

        cn.target_pos   = base_pos;
        cn.target_rot   = base_rot;
        cn.target_scale = base_scale;
        cn.target_z     = base_z;
    }
}

// =============================================================================
// animate - lerp each card toward its targets; call from _process()
// =============================================================================

void Cards::animate(double delta)
{
    float t = Math::clamp(
        static_cast<float>(ANIM_SPEED * delta), 0.f, 1.f);

    for (int i = 0; i < hand_cards.size(); i++)
    {
        CardNode& cn = hand_cards.write[i];
        if (!cn.root_ctrl) continue;
        if (i == dragged_index && drag_triggered) continue;

        Vector2 cur_pos   = cn.root_ctrl->get_position();
        float   cur_rot   = cn.root_ctrl->get_rotation_degrees();
        Vector2 cur_scale = cn.root_ctrl->get_scale();

        cn.root_ctrl->set_position(cur_pos.lerp(cn.target_pos, t));
        cn.root_ctrl->set_rotation_degrees(Math::lerp(cur_rot, cn.target_rot, t));
        cn.root_ctrl->set_scale(cur_scale.lerp(cn.target_scale, t));
        cn.root_ctrl->set_z_index(static_cast<int>(cn.target_z));
    }
}

// =============================================================================
// handle_input - hover detection, drag tracking, and release-to-use
// =============================================================================

bool Cards::handle_input(const Ref<InputEvent>& event)
{
    if (!cards_root) return false;

    // --- Mouse motion: update hover highlight or move dragged card ---
    Ref<InputEventMouseMotion> motion = event;
    if (motion.is_valid())
    {
        Vector2 local_mouse = _local_mouse_pos();

        if (dragged_index >= 0)
        {
            Vector2 delta_pos = local_mouse - drag_start_pos;
            if (!drag_triggered && delta_pos.length() > 8.f)
                drag_triggered = true;

            if (drag_triggered)
            {
                CardNode& cn = hand_cards.write[dragged_index];
                if (cn.root_ctrl)
                    cn.root_ctrl->set_position(local_mouse - drag_card_start);
            }
        }
        else
        {
            int new_hovered = -1;
            for (int i = 0; i < hand_cards.size(); i++)
            {
                const CardNode& cn = hand_cards[i];
                if (!cn.root_ctrl) continue;
                Rect2 rect(
                    cn.root_ctrl->get_position(),
                    cn.root_ctrl->get_size() * cn.root_ctrl->get_scale());
                if (rect.has_point(local_mouse))
                    new_hovered = i;
            }
            if (new_hovered != hovered_index)
            {
                hovered_index = new_hovered;
                layout();
            }
        }
        return true;
    }

    // --- Mouse button: begin drag on press, evaluate use on release ---
    Ref<InputEventMouseButton> btn = event;
    if (!btn.is_valid()) return false;
    if (btn->get_button_index() != MOUSE_BUTTON_LEFT) return false;

    if (btn->is_pressed())
    {
        Vector2 local_mouse = _local_mouse_pos();

        // Iterate from front (highest z) so topmost card wins
        for (int i = hand_cards.size() - 1; i >= 0; --i)
        {
            const CardNode& cn = hand_cards[i];
            if (!cn.root_ctrl) continue;
            Rect2 rect(
                cn.root_ctrl->get_position(),
                cn.root_ctrl->get_size() * cn.root_ctrl->get_scale());
            if (rect.has_point(local_mouse))
            {
                dragged_index   = i;
                drag_triggered  = false;
                drag_start_pos  = local_mouse;
                drag_card_start = local_mouse - cn.root_ctrl->get_position();
                cn.root_ctrl->move_to_front();
                return true;
            }
        }
    }
    else // button released
    {
        if (dragged_index >= 0 && drag_triggered)
        {
            Vector2 delta_pos = _local_mouse_pos() - drag_start_pos;
            if (-delta_pos.y >= DRAG_RELEASE_HEIGHT)
            {
                int used_index = dragged_index;
                dragged_index  = -1;
                drag_triggered = false;
                if (on_card_used_cb)
                {
                    // IMPORTANT: copy CardData by value BEFORE invoking the
                    // callback.  The callback (e.g. _on_card_used) may call
                    // hand.destroy() -> hand_cards.clear(), which would
                    // invalidate the reference and cause a SIGSEGV (signal 11)
                    // crash when Language (or any other enum) card is used.
                    CardData data_copy = hand_cards[used_index].data;
                    on_card_used_cb(used_index, data_copy);
                }
                return true;
            }
        }
        dragged_index  = -1;
        drag_triggered = false;
        layout();
    }
    return false;
}

// =============================================================================
// refresh_value_labels - re-read all setting values and update card centres
// =============================================================================

void Cards::refresh_value_labels()
{
    for (int i = 0; i < hand_cards.size(); i++)
    {
        CardNode& cn = hand_cards.write[i];
        if (!cn.value_label) continue;
        if (cn.data.type == CARD_BACK) continue;

        cn.data.value_text = _format_setting_value(cn.data);
        update_value_display(cn);
    }
}

// =============================================================================
// update_value_display - refresh the centre area of a single card
// =============================================================================

void Cards::update_value_display(CardNode& cn)
{
    if (cn.value_label)
        cn.value_label->set_text(cn.data.value_text);

    bool showing_image = false;
    if (cn.value_image)
    {
        String image_path = _get_value_image_path(cn.data);
        if (!image_path.is_empty())
        {
            Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(image_path);
            if (tex.is_valid())
            {
                cn.value_image->set_texture(tex);
                cn.value_image->set_visible(true);
                showing_image = true;
            }
        }
        if (!showing_image)
        {
            cn.value_image->set_texture(Ref<Texture2D>());
            cn.value_image->set_visible(false);
        }
    }

    // Hide the text label when an image is shown instead
    if (cn.value_label)
        cn.value_label->set_visible(!showing_image);

    // Bool widget (shown in addition to the value image/label)
    if (cn.widget_image)
    {
        String widget_path;
        if (cn.data.variant_type == Variant::BOOL && settings_ref)
        {
            bool current = static_cast<bool>(
                settings_ref->call(cn.data.prop_name + String("_getter")));
            widget_path = current
                ? String("res://Media/Image/card_true.png")
                : String("res://Media/Image/card_false.png");
        }
        if (!widget_path.is_empty())
        {
            Ref<Texture2D> wtex = ResourceLoader::get_singleton()->load(widget_path);
            if (wtex.is_valid())
            {
                cn.widget_image->set_texture(wtex);
                cn.widget_image->set_visible(true);
                return;
            }
        }
        cn.widget_image->set_texture(Ref<Texture2D>());
        cn.widget_image->set_visible(false);
    }
}

// =============================================================================
// _create_card - build all child nodes for a single card
// =============================================================================

CardNode Cards::_create_card(const CardData& data, int index)
{
    CardNode cn;
    cn.data = data;

    // --- Root Control ---
    Control* root = memnew(Control);
    root->set_name("Card_" + String::num_int64(index) + "_" + data.prop_name);
    root->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
    root->set_pivot_offset(Vector2(CARD_WIDTH * 0.5f, CARD_HEIGHT));
    root->set_clip_contents(false);
    cards_root->add_child(root);
    cn.root_ctrl = root;

    // --- Card frame TextureRect ---
    TextureRect* frame = memnew(TextureRect);
    frame->set_name("CardFrame");
    frame->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
    frame->set_position(Vector2(0.f, 0.f));
    frame->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
    frame->set_stretch_mode(TextureRect::STRETCH_SCALE);
    frame->set_modulate(Color(0.82f, 0.82f, 0.82f, 1.f));
    {
        Ref<Texture2D> frame_tex = ResourceLoader::get_singleton()->load(
            "res://Media/Image/card_frame.png");
        if (frame_tex.is_valid())
            frame->set_texture(frame_tex);
        else
            UtilityFunctions::printerr("Cards: Failed to load card_frame.png");
    }
    root->add_child(frame);

    // --- Transparent Panel (kept for compatibility) ---
    Panel* panel = memnew(Panel);
    panel->set_name("CardPanel");
    panel->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
    panel->set_position(Vector2(0.f, 0.f));
    {
        Ref<StyleBoxFlat> transparent = memnew(StyleBoxFlat);
        transparent->set_bg_color(Color(0.f, 0.f, 0.f, 0.f));
        transparent->set_border_width_all(0);
        panel->add_theme_stylebox_override("panel", transparent);
    }
    root->add_child(panel);
    cn.panel = panel;

    // --- Title label ---
    Label* title = memnew(Label);
    title->set_name("TitleLabel");
    title->set_text(data.display_name);
    title->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    title->set_text_overrun_behavior(TextServer::OVERRUN_NO_TRIMMING);
    title->add_theme_font_size_override("font_size", TITLE_FONT_SIZE);
    title->add_theme_color_override("font_color", dark_gray);
    root->add_child(title);
    title->set_position(Vector2(0.f, TITLE_Y));
    title->set_size(Vector2(CARD_WIDTH, TITLE_H));
    cn.title_label = title;

    // --- Central value label ---
    Label* val = memnew(Label);
    val->set_name("ValueLabel");
    val->set_text(data.value_text);
    val->set_position(Vector2(VALUE_X, CARD_HEIGHT * VALUE_Y_RATIO));
    val->set_size(Vector2(CARD_WIDTH - VALUE_W_MARGIN, CARD_HEIGHT * VALUE_H_RATIO));
    val->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    val->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
    val->add_theme_font_size_override("font_size", VALUE_FONT_SIZE);
    val->add_theme_color_override("font_color", dark_gray);
    val->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
    root->add_child(val);
    cn.value_label = val;

    // --- Central value image (shown instead of the label when a texture is found) ---
    TextureRect* value_image = memnew(TextureRect);
    value_image->set_name("ValueImage");
    value_image->set_position(Vector2(VALUE_X, CARD_HEIGHT * VALUE_Y_RATIO));
    value_image->set_size(Vector2(CARD_WIDTH - VALUE_W_MARGIN, CARD_HEIGHT * VALUE_H_RATIO));
    value_image->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
    value_image->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
    value_image->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    value_image->set_pivot_offset(value_image->get_size() * 0.5f);
    value_image->set_scale(Vector2(VALUE_IMAGE_SCALE, VALUE_IMAGE_SCALE));
    value_image->set_visible(false);
    root->add_child(value_image);
    cn.value_image = value_image;

    // --- Bool widget image ---
    TextureRect* widget_image = memnew(TextureRect);
    widget_image->set_name("WidgetImage");
    widget_image->set_position(Vector2(WIDGET_X, CARD_HEIGHT * WIDGET_Y_RATIO));
    widget_image->set_size(Vector2(WIDGET_W, WIDGET_H));
    widget_image->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
    widget_image->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
    widget_image->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    widget_image->set_pivot_offset(widget_image->get_size() * 0.5f);
    widget_image->set_scale(Vector2(WIDGET_IMAGE_SCALE, WIDGET_IMAGE_SCALE));
    widget_image->set_visible(false);
    root->add_child(widget_image);
    cn.widget_image = widget_image;

    // --- Detail / description label ---
    {
        // Derive the JSON key from the card data
        String key;
        if (data.type == CARD_MENU)
        {
            key = data.prop_name.to_lower();
            if (key.begins_with("menu_"))
                key = key.substr(5);
        }
        else if (!data.member_name.is_empty())
        {
            key = data.member_name;
        }
        else
        {
            key = data.prop_name;
        }

        String detail_text = get_json_text
            ? get_json_text("detail_" + key, "")
            : String("");

        Label* detail = memnew(Label);
        detail->set_name("DetailLabel");
        detail->set_text(detail_text);
        detail->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
        detail->set_vertical_alignment(VERTICAL_ALIGNMENT_TOP);
        detail->add_theme_font_size_override("font_size", DETAIL_FONT_SIZE);
        detail->add_theme_color_override("font_color", dark_gray);
        detail->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
        detail->add_theme_constant_override("line_spacing", DETAIL_LINE_SPACING);
        root->add_child(detail);
        detail->set_position(
            Vector2(DETAIL_W_MARGIN * 0.5f, CARD_HEIGHT * DETAIL_Y_RATIO));
        detail->set_size(
            Vector2(CARD_WIDTH - DETAIL_W_MARGIN, CARD_HEIGHT * DETAIL_H_RATIO));
        cn.detail_label = detail;
    }

    // --- Type badge (top-right corner) ---
    Label* type_lbl = memnew(Label);
    type_lbl->set_name("TypeLabel");
    String type_char;
    if      (data.type == CARD_BACK)             type_char = "<";  // back arrow (ASCII substitute)
    else if (data.type == CARD_OPTION)           type_char = "v";  // checkmark (ASCII substitute)
    else if (data.type == CARD_MENU)             type_char = "M";
    else if (data.type == CARD_ESCAPE_RETURN)    type_char = "R";
    else if (data.type == CARD_ESCAPE_CONTINUE)  type_char = "C";
    else if (data.variant_type == Variant::BOOL) type_char = "B";
    else                                          type_char = "E";
    type_lbl->set_text(type_char);
    type_lbl->set_position(Vector2(TYPE_LABEL_X, TYPE_LABEL_Y));
    type_lbl->set_size(Vector2(TYPE_LABEL_W, TYPE_LABEL_H));
    type_lbl->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    {
        Ref<SystemFont> type_font;
        type_font.instantiate();
        PackedStringArray font_names;
        font_names.push_back("Segoe UI");
        font_names.push_back("Arial");
        type_font->set_font_names(font_names);
        type_font->set_font_weight(700);
        type_font->set_allow_system_fallback(true);
        type_lbl->add_theme_font_override("font", type_font);
    }
    type_lbl->add_theme_font_size_override("font_size", TYPE_LABEL_FONT_SIZE);
    type_lbl->add_theme_color_override("font_color", Color(0.1f, 0.2125f, 0.25f, 1.f));
    root->add_child(type_lbl);

    // --- Debug region overlays ---
    if (debug_ui_flag)
    {
        auto make_overlay = [&](Vector2 pos, Vector2 size, Color col)
        {
            Panel* overlay = memnew(Panel);
            overlay->set_position(pos);
            overlay->set_size(size);
            Ref<StyleBoxFlat> s = memnew(StyleBoxFlat);
            s->set_bg_color(col);
            s->set_border_width_all(0);
            overlay->add_theme_stylebox_override("panel", s);
            root->add_child(overlay);
        };

        make_overlay(
            Vector2(TITLE_X, TITLE_Y),
            Vector2(CARD_WIDTH - TITLE_W_MARGIN, TITLE_H),
            Color(0.f, 1.f, 0.f, 0.25f));  // green = title

        make_overlay(
            Vector2(VALUE_X, CARD_HEIGHT * VALUE_Y_RATIO),
            Vector2(CARD_WIDTH - VALUE_W_MARGIN, CARD_HEIGHT * VALUE_H_RATIO),
            Color(0.f, 0.4f, 1.f, 0.25f));  // blue = value

        make_overlay(
            Vector2(WIDGET_X, CARD_HEIGHT * WIDGET_Y_RATIO),
            Vector2(WIDGET_W, WIDGET_H),
            Color(0.f, 0.75f, 0.8f, 0.25f)); // cyan = widget

        make_overlay(
            Vector2(DETAIL_X, CARD_HEIGHT * DETAIL_Y_RATIO),
            Vector2(CARD_WIDTH - DETAIL_W_MARGIN, CARD_HEIGHT * DETAIL_H_RATIO),
            Color(1.f, 0.5f, 0.f, 0.25f));  // orange = detail

        make_overlay(
            Vector2(TYPE_LABEL_X, TYPE_LABEL_Y),
            Vector2(TYPE_LABEL_W, TYPE_LABEL_H),
            Color(1.f, 0.f, 0.f, 0.35f));   // red = type badge
    }

    // --- Badge icon overlay (shows_badge cards only) ---
    if (data.shows_badge)
    {
        TextureRect* badge = memnew(TextureRect);
        badge->set_name("Badge_Icon");
        badge->set_position(Vector2(7.f, 8.f));
        badge->set_size(Vector2(25.f, 25.f));
        badge->set_expand_mode(TextureRect::EXPAND_FIT_WIDTH_PROPORTIONAL);
        badge->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
        badge->set_texture_filter(CanvasItem::TEXTURE_FILTER_LINEAR);
        badge->set_visible(false);
        root->add_child(badge);
        badge_icon = badge;
    }

    update_value_display(cn);
    return cn;
}

// =============================================================================
// _local_mouse_pos
// =============================================================================

Vector2 Cards::_local_mouse_pos() const
{
    if (!cards_root) return Vector2();
    return cards_root->get_local_mouse_position();
}

// =============================================================================
// _get_value_image_path
// =============================================================================

String Cards::_get_value_image_path(const CardData& data) const
{
    const String base = "res://Media/Image/";

    if (data.type == CARD_BACK)            return base + String("card_back.png");
    if (data.type == CARD_ESCAPE_RETURN)   return base + String("card_return.png");
    if (data.type == CARD_ESCAPE_CONTINUE) return base + String("card_continue.png");

    if (data.type == CARD_MENU && !data.member_name.is_empty())
        return base + String("card_") + data.member_name + String(".png");

    if (data.variant_type == Variant::BOOL && !data.member_name.is_empty())
        return base + String("card_") + data.member_name + String(".png");

    const bool is_language =
        (data.prop_name == "Language" || data.member_name == "language");

    if (is_language)
    {
        int lang_value = -1;
        if (data.type == CARD_OPTION)
            lang_value = data.option_value;
        else if (settings_ref)
            lang_value = settings_ref->GetValLanguage();

        if (lang_value >= 0)
            return (lang_value == 1)
                ? base + String("card_zh.png")
                : base + String("card_en.png");
    }

    return "";
}

// =============================================================================
// _format_setting_value - read the current value from Settings and format it
// =============================================================================

String Cards::_format_setting_value(const CardData& data) const
{
    if (!settings_ref) return "";

    Variant val = settings_ref->call(data.prop_name + String("_getter"));

    if (data.variant_type == Variant::BOOL)
    {
        return static_cast<bool>(val) ? "ON" : "OFF";
    }
    else if (data.hint == PROPERTY_HINT_ENUM)
    {
        int idx = static_cast<int>(val);
        PackedStringArray items = data.hint_string.split(",", false);
        if (idx >= 0 && idx < items.size())
            return items[idx].strip_edges();
        return String::num_int64(idx);
    }
    else
    {
        return String(val);
    }
}