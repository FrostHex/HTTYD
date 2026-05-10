// ==================== Control_Scene_Home.cpp ====================
//
// 设置面板已重构为手牌卡牌系统：
//   - 主页菜单同样采用手牌卡牌交互
//   - 所有设置项以卡牌形式呈扇形排列在面板底部
//   - 鼠标悬停：卡牌摆正放大并浮出其他卡牌
//   - 左键按住向上拖拽超过阈值后松开：触发"使用"
//   - bool 类型：使用卡牌后切换值，卡牌留在手中并刷新显示
//   - enum / int(range) 类型：清空手牌，展示返回牌 + 子选项牌
//   - 选中子选项或返回牌：清空手牌，重新展示主手牌
//
// ================================================================

#include "Control_Scene_Home.h"
#include "Settings.h"
#include "Control_Main.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/color.hpp>
#include <regex>
#include <cmath>

using namespace godot;

// ================================================================
// 构造 / 析构
// ================================================================

Control_Scene_Home::Control_Scene_Home()
{
}

Control_Scene_Home::~Control_Scene_Home()
{
}

// ================================================================
// _ready
// ================================================================

void Control_Scene_Home::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    // ── 获取 Settings 单例 ──────────────────────────────────
    settings = Settings::GetSingleton();
    if (!settings)
    {
        UtilityFunctions::printerr("Control_Scene_Home: Could not find Settings singleton");
        return;
    }

    // ── 获取 Control_Main 引用 ──────────────────────────────
    SceneTree *tree = get_tree();
    if (tree)
    {
        Window *root = tree->get_root();
        if (root)
        {
            control_main = Object::cast_to<Control_Main>(
                root->get_node_or_null(NodePath("Main/Control_Main")));
            if (!control_main)
            {
                UtilityFunctions::printerr(
                    "Control_Scene_Home: Could not find Control_Main at Main/Control_Main");
            }
        }
    }

    // ── 处理 XR / 普通视口 ──────────────────────────────────
    viewport_container = get_parent()->get_node<Node>("SubViewportContainer");
    if (settings->GetValEnableHeadset())
    {
        Node* canvas_layer = viewport_container->get_node<Node>("Viewport/CanvasLayer");
        viewport_container = get_parent()->get_node<Node>("XRToolsViewport2DIn3D");
        canvas_layer->reparent(viewport_container->get_node<Node>("Viewport"));

        Ref<XRInterface> primary = XRServer::get_singleton()->get_primary_interface();
        bool xr_active = primary.is_valid() && primary->is_initialized();
        if (!xr_active)
        {
            Node *xr_viewport    = viewport_container->get_node<Node>("Viewport");
            Node *canvas_in_xr   = xr_viewport->get_node<Node>("CanvasLayer");
            Node *base_container = get_parent()->get_node<Node>("SubViewportContainer");
            Node *base_viewport  = base_container->get_node<Node>("Viewport");
            canvas_in_xr->reparent(base_viewport);
            viewport_container = base_container;
            UtilityFunctions::print("XR initialization failed, restored UI to SubViewportContainer.");
        }
    }

    // ── 获取 UI 根节点与设置面板 ──────────────────────────────
    Node *ui_root_node = viewport_container->get_node_or_null(
        NodePath("Viewport/CanvasLayer/Control"));
    ui_root = Object::cast_to<Control>(ui_root_node);
    if (!ui_root)
    {
        UtilityFunctions::printerr(
            "Control_Scene_Home: Could not find UI root at Viewport/CanvasLayer/Control");
        return;
    }

    // settings_panel 仍然作为整体面板的 visible 开关
    settings_panel = ui_root->get_node_or_null(NodePath("Settings_Panel"));
    if (!settings_panel)
    {
        UtilityFunctions::printerr(
            "Control_Scene_Home: Could not find Settings_Panel under UI root");
    }

    // ── 构建卡牌手牌根节点并建立主菜单 ──────────────────────
    _build_card_hand_root();
    _rebuild_menu_hand();
}

// ================================================================
// _process — 每帧插值动画
// ================================================================

void Control_Scene_Home::_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint()) return;

    _animate_hand(delta);
}

// ================================================================
// _input — 处理鼠标悬停 / 拖拽 / 松开
// ================================================================

void Control_Scene_Home::_input(const Ref<InputEvent>& event)
{
    if (Engine::get_singleton()->is_editor_hint()) return;
    if (!card_hand_root) return;

    bool settings_visible = settings_panel && static_cast<bool>(settings_panel->get("visible"));
    if ((hand_state == HAND_SETTINGS_MAIN || hand_state == HAND_SETTINGS_SUB) && !settings_visible)
        return;
    if (hand_state == HAND_MENU && settings_visible)
        return;

    // ── 鼠标移动：检测悬停 ──────────────────────────────────
    Ref<InputEventMouseMotion> motion = event;
    if (motion.is_valid())
    {
        Vector2 local_mouse = _get_hand_local_mouse_pos();
        if (dragged_index >= 0)
        {
            // 正在拖拽：更新卡牌位置
            Vector2 delta_pos = local_mouse - drag_start_pos;
            if (!drag_triggered && delta_pos.length() > 8.f)
                drag_triggered = true;

            if (drag_triggered && card_hand_root)
            {
                CardNode& cn = hand_cards.write[dragged_index];
                if (cn.root_ctrl)
                {
                    // 让卡牌跟随鼠标，保持按下时相对偏移
                    cn.root_ctrl->set_position(local_mouse - drag_card_start);
                }
            }
        }
        else
        {
            // 无拖拽：检测悬停
            int new_hovered = -1;
            if (card_hand_root)
            {
                for (int i = 0; i < hand_cards.size(); i++)
                {
                    const CardNode& cn = hand_cards[i];
                    if (!cn.root_ctrl) continue;
                    Rect2 rect(cn.root_ctrl->get_position(),
                               cn.root_ctrl->get_size() * cn.root_ctrl->get_scale());
                    if (rect.has_point(local_mouse))
                        new_hovered = i;
                }
            }

            if (new_hovered != hovered_index)
            {
                hovered_index = new_hovered;
                _layout_hand(); // 重新计算 target
            }
        }
        return;
    }

    // ── 鼠标按钮 ────────────────────────────────────────────
    Ref<InputEventMouseButton> btn = event;
    if (!btn.is_valid()) return;
    if (btn->get_button_index() != MOUSE_BUTTON_LEFT) return;

    if (btn->is_pressed())
    {
        // 按下：检测是否落在某张卡牌上
        if (card_hand_root)
        {
            Vector2 local_mouse = _get_hand_local_mouse_pos();
            for (int i = hand_cards.size() - 1; i >= 0; --i)
            {
                const CardNode& cn = hand_cards[i];
                if (!cn.root_ctrl) continue;
                Rect2 rect(cn.root_ctrl->get_position(),
                           cn.root_ctrl->get_size() * cn.root_ctrl->get_scale());
                if (rect.has_point(local_mouse))
                {
                    dragged_index    = i;
                    drag_triggered   = false;
                    drag_start_pos   = local_mouse;

                    // 记录鼠标相对卡牌左上角的偏移（局部坐标系）
                    drag_card_start = local_mouse - cn.root_ctrl->get_position();

                    // 将被拖拽卡牌提到最顶层
                    cn.root_ctrl->move_to_front();
                    break;
                }
            }
        }
    }
    else
    {
        // 松开：判断是否上移足够距离触发"使用"
        if (dragged_index >= 0 && drag_triggered)
        {
            Vector2 delta_pos = _get_hand_local_mouse_pos() - drag_start_pos;
            if (-delta_pos.y >= drag_release_height)
            {
                // 触发使用
                int used_index = dragged_index;
                dragged_index  = -1;
                drag_triggered = false;
                _on_card_used(used_index);
                return;
            }
        }

        // 未触发使用：卡牌弹回原位（通过 _layout_hand 重算 target）
        dragged_index  = -1;
        drag_triggered = false;
        _layout_hand();
    }
}

// ================================================================
// ── 卡牌手牌构建 ────────────────────────────────────────────────
// ================================================================

// ── 创建手牌根节点（锚定到底部中央）────────────────────────────

void Control_Scene_Home::_build_card_hand_root()
{
    if (card_hand_root || !ui_root) return;

    // 在 UI 根节点下创建一个 Control 作为所有卡牌的容器
    Control* root = memnew(Control);
    root->set_name("CardHandRoot");
    root->set_anchor_and_offset(SIDE_LEFT,   0.5f, 0.f);
    root->set_anchor_and_offset(SIDE_RIGHT,  0.5f, 0.f);
    root->set_anchor_and_offset(SIDE_BOTTOM, 1.0f, -10.f);
    root->set_anchor_and_offset(SIDE_TOP,    1.0f, -10.f);
    root->set_size(Vector2(0, 0));
    // 不裁剪子节点，卡牌可超出范围显示
    root->set_clip_contents(false);
    // 最近邻插值，保持文字和纹理清晰
    root->set_texture_filter(Control::TEXTURE_FILTER_NEAREST);

    ui_root->add_child(root);
    card_hand_root = root;
}

// ── 切换手牌宿主（菜单 / 设置面板）──────────────────────────────

void Control_Scene_Home::_set_hand_host(Node* host)
{
    if (!card_hand_root || !host) return;

    if (card_hand_root->get_parent() != host)
    {
        card_hand_root->reparent(host);
    }

    Control* root = Object::cast_to<Control>(card_hand_root);
    if (!root) return;

    root->set_anchor_and_offset(SIDE_LEFT,   0.5f, 0.f);
    root->set_anchor_and_offset(SIDE_RIGHT,  0.5f, 0.f);
    root->set_anchor_and_offset(SIDE_BOTTOM, 1.0f, -10.f);
    root->set_anchor_and_offset(SIDE_TOP,    1.0f, -10.f);
    root->set_size(Vector2(0, 0));
}

Vector2 Control_Scene_Home::_get_hand_local_mouse_pos() const
{
    Control* root = Object::cast_to<Control>(card_hand_root);
    if (!root) return Vector2();

    return root->get_local_mouse_position();
}

// ── 重建主菜单手牌 ───────────────────────────────────────────────

void Control_Scene_Home::_rebuild_menu_hand()
{
    _destroy_hand();
    hand_state = HAND_MENU;
    sub_parent_prop = "";

    if (!card_hand_root) return;
    _set_hand_host(ui_root);

    struct MenuEntry
    {
        const char* id;
        const char* key;
        const char* fallback;
        const char* scene;
        bool opens_settings;
        bool shows_badge;
    };

    const MenuEntry entries[] = {
        {"Menu_Settings", "button_settings", "Settings", "", true,  false},
        {"Menu_Tutorial", "button_tutorial", "Note Book", "Scene_Tutorial", false, false},
        {"Menu_Practice", "button_practice", "Flight Practice", "Scene_Practice", false, false},
        {"Menu_TD",       "button_td",      "Test Drive", "Scene_TD", false, true},
        {"Menu_Dodge",    "button_dodge",   "Dodge", "Scene_Dodge", false, false},
    };

    for (const MenuEntry& entry : entries)
    {
        CardData data;
        data.type          = CARD_MENU;
        data.prop_name     = entry.id;
        data.display_name  = _get_json_text(entry.key, entry.fallback);
        data.value_text    = data.display_name;
        data.option_value  = 0;
        data.variant_type  = Variant::NIL;
        data.hint          = PROPERTY_HINT_NONE;
        data.hint_string   = "";
        data.is_custom     = false;
        data.scene_name    = entry.scene;
        data.opens_settings = entry.opens_settings;
        data.shows_badge   = entry.shows_badge;

        CardNode cn = _create_card_node(data, hand_cards.size());
        hand_cards.push_back(cn);
    }

    _layout_hand();
    _update_badge_display();
}

// ── 重建主手牌（所有 exposed 设置项） ─────────────────────────────

void Control_Scene_Home::_rebuild_settings_main_hand()
{
    _destroy_hand();
    hand_state = HAND_SETTINGS_MAIN;
    sub_parent_prop = "";

    if (!settings) return;
    if (!settings_panel) return;
    _set_hand_host(settings_panel);
    current_settings_list = settings->GetExposedSettings();

    // ── 返回牌（最左边） ─────────────────────────────────────
    {
        CardData back;
        back.type         = CARD_BACK;
        back.prop_name    = "";
        back.display_name = _get_json_text("button_back", "Back");
        back.value_text   = _get_json_text("button_back", "Back");
        back.variant_type = Variant::NIL;
        back.hint         = PROPERTY_HINT_NONE;
        back.hint_string  = "";
        back.is_custom    = false;
        back.scene_name   = "";
        back.opens_settings = false;
        back.shows_badge  = false;
        back.option_value = 0;

        CardNode cn = _create_card_node(back, hand_cards.size());
        hand_cards.push_back(cn);
    }

    for (int i = 0; i < current_settings_list.size(); i++)
    {
        Dictionary entry  = current_settings_list[i];
        String prop_name  = String(entry.get("prop_name", ""));
        if (prop_name.is_empty()) continue;

        CardData data;
        data.type         = CARD_SETTING;
        data.prop_name    = prop_name;
        data.display_name = _get_setting_display_name(String(entry.get("member", "")));
        data.variant_type = static_cast<int>(entry.get("variant_type", Variant::NIL));
        data.hint         = static_cast<int>(entry.get("hint", PROPERTY_HINT_NONE));
        data.hint_string  = String(entry.get("hint_string", ""));
        data.is_custom    = static_cast<bool>(entry.get("is_custom", false));
        data.scene_name   = "";
        data.opens_settings = false;
        data.shows_badge  = false;
        data.option_value = 0;
        data.value_text   = _format_setting_value(data);

        CardNode cn = _create_card_node(data, hand_cards.size());
        hand_cards.push_back(cn);
    }

    _layout_hand();
}

// ── 重建子手牌（enum / int range 展开） ──────────────────────────

void Control_Scene_Home::_rebuild_sub_hand(
    const String& prop_name, const CardData& parent_data)
{
    _destroy_hand();
    hand_state      = HAND_SETTINGS_SUB;
    sub_parent_prop = prop_name;

    // ── 返回牌（最左边） ─────────────────────────────────────
    {
        CardData back;
        back.type         = CARD_BACK;
        back.prop_name    = prop_name;
        back.display_name = _get_json_text("button_back", "Back");
        back.value_text   = _get_json_text("button_back", "Back");
        back.variant_type = parent_data.variant_type;
        back.hint         = parent_data.hint;
        back.hint_string  = parent_data.hint_string;
        back.is_custom    = parent_data.is_custom;
        back.scene_name   = "";
        back.opens_settings = false;
        back.shows_badge  = false;
        back.option_value = 0;

        CardNode cn = _create_card_node(back, hand_cards.size());
        hand_cards.push_back(cn);
    }

    // ── 子选项牌 ─────────────────────────────────────────────
    if (parent_data.hint == PROPERTY_HINT_ENUM)
    {
        // enum：按 hint_string 中的逗号分割选项
        PackedStringArray items = parent_data.hint_string.split(",", false);
        for (int idx = 0; idx < items.size(); idx++)
        {
            CardData opt;
            opt.type          = CARD_OPTION;
            opt.prop_name     = prop_name;
            opt.display_name  = parent_data.display_name;
            opt.value_text    = items[idx].strip_edges();
            opt.variant_type  = parent_data.variant_type;
            opt.hint          = parent_data.hint;
            opt.hint_string   = parent_data.hint_string;
            opt.is_custom     = parent_data.is_custom;
            opt.scene_name    = "";
            opt.opens_settings = false;
            opt.shows_badge   = false;
            opt.option_value  = idx;

            CardNode cn = _create_card_node(opt, hand_cards.size());
            hand_cards.push_back(cn);
        }
    }
    else if (parent_data.hint == PROPERTY_HINT_RANGE)
    {
        // int range：解析 "min,max" 并枚举每个整数值
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
            opt.type          = CARD_OPTION;
            opt.prop_name     = prop_name;
            opt.display_name  = parent_data.display_name;
            opt.value_text    = String::num_int64(v);
            opt.variant_type  = parent_data.variant_type;
            opt.hint          = parent_data.hint;
            opt.hint_string   = parent_data.hint_string;
            opt.is_custom     = parent_data.is_custom;
            opt.scene_name    = "";
            opt.opens_settings = false;
            opt.shows_badge   = false;
            opt.option_value  = v;

            CardNode cn = _create_card_node(opt, hand_cards.size());
            hand_cards.push_back(cn);
        }
    }

    _layout_hand();
}

// ── 创建单张卡牌节点 ─────────────────────────────────────────────

Control_Scene_Home::CardNode
Control_Scene_Home::_create_card_node(const CardData& data, int index)
{
    CardNode cn;
    cn.data = data;

    // ── 根 Control ───────────────────────────────────────────
    Control* root = memnew(Control);
    root->set_name("Card_" + String::num_int64(index) + "_" + data.prop_name);
    root->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
    // pivot 设在卡牌底部中央，方便扇形旋转
    root->set_pivot_offset(Vector2(CARD_WIDTH * 0.5f, CARD_HEIGHT));
    root->set_clip_contents(false);
    card_hand_root->add_child(root);
    cn.root_ctrl = root;

    // ── 卡牌底板 Panel ───────────────────────────────────────
    Panel* panel = memnew(Panel);
    panel->set_name("CardPanel");
    panel->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
    panel->set_position(Vector2(0, 0));

    // 根据卡牌类型设置不同底色
    Ref<StyleBoxFlat> style = memnew(StyleBoxFlat);
    style->set_corner_radius_all(10);
    style->set_border_width_all(2);

    // 统一黄色/金色主题（与 Language 牌一致）
    style->set_bg_color(Color(0.22f, 0.18f, 0.08f, 0.95f));
    style->set_border_color(Color(0.90f, 0.70f, 0.20f, 1.f));

    panel->add_theme_stylebox_override("panel", style);
    root->add_child(panel);
    cn.panel = panel;

    // ── 顶部标题 Label ───────────────────────────────────────
    Label* title = memnew(Label);
    title->set_name("TitleLabel");
    title->set_text(data.display_name);
    title->set_position(Vector2(4.f, 6.f));
    title->set_size(Vector2(CARD_WIDTH - 8.f, 28.f));
    title->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    title->add_theme_font_size_override("font_size", 15);
    title->add_theme_color_override("font_color", Color(0.95f, 0.88f, 0.60f, 1.f));
    root->add_child(title);
    cn.title_label = title;

    // ── 中央值 Label ─────────────────────────────────────────
    Label* val = memnew(Label);
    val->set_name("ValueLabel");
    val->set_text(data.value_text);
    val->set_position(Vector2(4.f, CARD_HEIGHT * 0.38f));
    val->set_size(Vector2(CARD_WIDTH - 8.f, CARD_HEIGHT * 0.35f));
    val->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    val->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
    val->add_theme_font_size_override("font_size", 17);
    val->add_theme_color_override("font_color", Color(1.f, 1.f, 1.f, 1.f));
    val->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
    root->add_child(val);
    cn.value_label = val;

    // ── 底部装饰线 ───────────────────────────────────────────
    // 用一个细长 Panel 模拟卡牌底部的装饰横线
    Panel* deco = memnew(Panel);
    deco->set_name("DecoLine");
    deco->set_position(Vector2(8.f, CARD_HEIGHT - 30.f));
    deco->set_size(Vector2(CARD_WIDTH - 16.f, 2.f));
    Ref<StyleBoxFlat> deco_style = memnew(StyleBoxFlat);
    deco_style->set_bg_color(Color(0.80f, 0.65f, 0.20f, 0.6f));
    deco->add_theme_stylebox_override("panel", deco_style);
    root->add_child(deco);

    // ── 底部费用角标（类型标识）────────────────────────────────
    // 此处展示类型首字母
    Label* cost = memnew(Label);
    cost->set_name("CostLabel");
    String type_char = "?";
    if (data.type == CARD_BACK)              type_char = "←";
    else if (data.type == CARD_OPTION)       type_char = "✓";
    else if (data.type == CARD_MENU)         type_char = "M";
    else if (data.variant_type == Variant::BOOL) type_char = "B";
    else                                     type_char = "E";
    cost->set_text(type_char);
    cost->set_position(Vector2(4.f, CARD_HEIGHT - 26.f));
    cost->set_size(Vector2(22.f, 22.f));
    cost->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
    cost->add_theme_font_size_override("font_size", 18);
    cost->add_theme_color_override("font_color", Color(0.4f, 0.85f, 1.f, 1.f));
    root->add_child(cost);

    if (data.shows_badge)
    {
        TextureRect* badge = memnew(TextureRect);
        badge->set_name("Badge_Icon");
        badge->set_position(Vector2(CARD_WIDTH - 33.f, -9.f));
        badge->set_size(Vector2(45.f, 45.f));
        badge->set_expand_mode(TextureRect::EXPAND_FIT_WIDTH_PROPORTIONAL);
        badge->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
        badge->set_texture_filter(CanvasItem::TEXTURE_FILTER_LINEAR);
        badge->set_visible(false);
        root->add_child(badge);
        badge_icon = badge;
    }

    return cn;
}

// ── 销毁所有手牌节点 ─────────────────────────────────────────────

void Control_Scene_Home::_destroy_hand()
{
    for (int i = 0; i < hand_cards.size(); i++)
    {
        if (hand_cards[i].root_ctrl)
        {
            hand_cards[i].root_ctrl->queue_free();
        }
    }
    hand_cards.clear();
    hovered_index  = -1;
    dragged_index  = -1;
    drag_triggered = false;
    badge_icon     = nullptr;
}

// ================================================================
// ── 布局计算 ────────────────────────────────────────────────────
// ================================================================

// 计算每张卡牌在扇形手牌中的目标位置和旋转角度
// 卡牌以面板底部中央为圆心呈扇形排列，
// 每张卡牌的 pivot 在底部中央，因此旋转时自然形成扇形。

void Control_Scene_Home::_layout_hand()
{
    int n = hand_cards.size();
    if (n == 0) return;

    // 扇形角度范围：由 FAN_MAX_ANGLE_DEG 定义
    float total_angle = FAN_MAX_ANGLE_DEG;
    float step = (n > 1) ? total_angle / static_cast<float>(n - 1) : 0.f;

    for (int i = 0; i < n; i++)
    {
        CardNode& cn = hand_cards.write[i];

        // 该牌在扇形中的角度（从 -total_angle/2 到 +total_angle/2）
        float angle_deg = -total_angle * 0.5f + step * static_cast<float>(i);
        float angle_rad = Math::deg_to_rad(angle_deg);

        // 以圆弧为中心线：卡牌底部中心落在同一圆弧上
        float x_offset = FAN_RADIUS * Math::sin(angle_rad);
        float y_offset = FAN_RADIUS * (1.f - Math::cos(angle_rad));

        // 基础目标：卡牌底部中央对准 (x_offset, 0)
        // root_ctrl 的 pivot 在 (CARD_WIDTH/2, CARD_HEIGHT)
        Vector2 base_pos(
            x_offset - CARD_WIDTH * 0.5f,
            -CARD_HEIGHT + y_offset);

        float base_rot   = angle_deg;
        Vector2 base_scale(1.f, 1.f);
        float base_z     = static_cast<float>(i);

        if (i == hovered_index && dragged_index < 0)
        {
            // 悬停：摆正、放大、上浮、z 最高
            base_pos.y  -= HOVER_LIFT;
            base_rot     = 0.f;
            base_scale   = Vector2(HOVER_SCALE, HOVER_SCALE);
            // 悬停牌居中显示，调整 x 避免缩放后超出
            base_z       = static_cast<float>(n + 10);
        }

        cn.target_pos   = base_pos;
        cn.target_rot   = base_rot;
        cn.target_scale = base_scale;
        cn.target_z     = base_z;
    }
}

// ── 每帧插值逼近目标 ─────────────────────────────────────────────

void Control_Scene_Home::_animate_hand(double delta)
{
    float t = Math::clamp(
        static_cast<float>(ANIM_SPEED * delta), 0.f, 1.f);

    for (int i = 0; i < hand_cards.size(); i++)
    {
        CardNode& cn = hand_cards.write[i];
        if (!cn.root_ctrl) continue;

        // 被拖拽的牌由 _input 直接控制，跳过插值
        if (i == dragged_index && drag_triggered) continue;

        Vector2 cur_pos   = cn.root_ctrl->get_position();
        float   cur_rot   = cn.root_ctrl->get_rotation_degrees();
        Vector2 cur_scale = cn.root_ctrl->get_scale();

        cn.root_ctrl->set_position(cur_pos.lerp(cn.target_pos, t));
        cn.root_ctrl->set_rotation_degrees(
            Math::lerp(cur_rot, cn.target_rot, t));
        cn.root_ctrl->set_scale(cur_scale.lerp(cn.target_scale, t));

        // z_index 直接设置（不需要插值）
        cn.root_ctrl->set_z_index(static_cast<int>(cn.target_z));
    }
}

// ================================================================
// ── 卡牌交互逻辑 ────────────────────────────────────────────────
// ================================================================

void Control_Scene_Home::_on_card_used(int index)
{
    if (index < 0 || index >= hand_cards.size()) return;

    const CardData& data = hand_cards[index].data;

    if (data.type == CARD_MENU)
    {
        if (data.opens_settings)
        {
            _on_settings_button_pressed();
        }
        else if (!data.scene_name.is_empty())
        {
            _on_button_pressed(data.scene_name);
        }
        return;
    }

    if (data.type == CARD_BACK)
    {
        if (hand_state == HAND_SETTINGS_MAIN)
        {
            // 返回牌在设置主手牌中：关闭设置面板，返回主菜单
            if (settings_panel)
                settings_panel->set("visible", false);
            _set_hand_host(ui_root);
            _rebuild_menu_hand();
        }
        else
        {
            // 返回牌在子手牌中：返回主手牌
            _rebuild_settings_main_hand();
        }
        return;
    }

    if (data.type == CARD_OPTION)
    {
        // 应用选项并返回主手牌
        _apply_option_select(data);
        _rebuild_settings_main_hand();
        return;
    }

    // data.type == CARD_SETTING
    if (data.variant_type == Variant::BOOL)
    {
        // bool：切换值，卡牌留在手中，刷新显示
        _apply_bool_toggle(data);
        _update_settings_main_value_labels();
        return;
    }
    else
    {
        // enum / int range：展开子手牌
        _rebuild_sub_hand(data.prop_name, data);
        return;
    }
}

// ── 切换 bool 设置 ───────────────────────────────────────────────

void Control_Scene_Home::_apply_bool_toggle(const CardData& data)
{
    if (!settings) return;

    bool current = static_cast<bool>(settings->call(data.prop_name + String("_getter")));
    settings->call(data.prop_name + String("_setter"), !current);

    // EnableHeadset / VolumetricClouds 有互斥逻辑（由 Settings 内部处理），
    // 切换后刷新所有主手牌卡面
    if (data.prop_name == "Language")
        _on_language_changed();
}

// ── 应用 enum / int 选项 ─────────────────────────────────────────

void Control_Scene_Home::_apply_option_select(const CardData& data)
{
    if (!settings) return;

    settings->call(data.prop_name + String("_setter"), data.option_value);

    if (data.prop_name == "Language")
        _on_language_changed();
}

// ── 刷新主手牌所有卡牌的中央值文字 ───────────────────────────────

void Control_Scene_Home::_update_settings_main_value_labels()
{
    if (hand_state != HAND_SETTINGS_MAIN) return;

    for (int i = 0; i < hand_cards.size(); i++)
    {
        CardNode& cn = hand_cards.write[i];
        if (!cn.value_label) continue;
        if (cn.data.type == CARD_BACK) continue;

        String new_val = _format_setting_value(cn.data);
        cn.data.value_text = new_val;
        cn.value_label->set_text(new_val);
    }
}

// ================================================================
// ── 工具函数 ────────────────────────────────────────────────────
// ================================================================

// CamelCase → "Camel Case"（含全大写缩写的处理）
// 例："VolumetricClouds" → "Volumetric Clouds"
//     "SSRQualityLevel"  → "SSR Quality Level"
/*static*/
String Control_Scene_Home::_camel_to_display(const String& name)
{
    std::string s(name.utf8().get_data());
    // ABC Def → "AB C Def" pass 1: 全大写+首字母大写
    s = std::regex_replace(s, std::regex("([A-Z]+)([A-Z][a-z])"), "$1 $2");
    // abcDef → "abc Def" pass 2: 小写/数字 紧接大写
    s = std::regex_replace(s, std::regex("([a-z0-9])([A-Z])"), "$1 $2");
    return String(s.c_str());
}

// 读取设置项当前值并格式化为字符串
String Control_Scene_Home::_format_setting_value(const CardData& data) const
{
    if (!settings) return "";

    Variant val = settings->call(data.prop_name + String("_getter"));

    if (data.variant_type == Variant::BOOL)
    {
        return static_cast<bool>(val) ? "ON" : "OFF";
    }
    else if (data.hint == PROPERTY_HINT_ENUM)
    {
        // 将整数索引映射为 hint_string 中的文字
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

// ================================================================
// ── 读取多语言 JSON 文本 ─────────────────────────────────────────
// ================================================================

// 获取设置项的翻译显示名称
String Control_Scene_Home::_get_setting_display_name(const String& member_name)
{
    // 先尝试从 JSON 获取翻译
    String key = "entry_" + member_name.to_lower();
    String translated = _get_json_text(key, "");
    if (!translated.is_empty())
        return translated;
    // 回退到 CamelCase 转换
    return _camel_to_display(member_name);
}

String Control_Scene_Home::_get_json_text(
    const String& key, const String& fallback)
{
    if (!settings) return fallback;

    String json_file = (settings->GetValLanguage() == 1)
        ? "res://Media/Text/Chinese.json"
        : "res://Media/Text/English.json";

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
}

// ================================================================
// ── 普通场景按钮回调 ────────────────────────────────────────────
// ================================================================

void Control_Scene_Home::_on_button_pressed(const String& scene_name)
{
    if (control_main)
    {
        UtilityFunctions::print("Switching to scene: ", scene_name);
        control_main->Switch_Scene(scene_name);
    }
    else
    {
        UtilityFunctions::printerr("Control_Main not available for scene switch");
    }
}

void Control_Scene_Home::_on_settings_button_pressed()
{
    if (settings_panel)
    {
        settings_panel->set("visible", true);
        // 面板打开时重建主手牌，确保显示最新值
        _set_hand_host(settings_panel);
        _rebuild_settings_main_hand();
    }
}

void Control_Scene_Home::_on_close_button_pressed()
{
    if (settings_panel)
    {
        settings_panel->set("visible", false);
    }

    _set_hand_host(ui_root);
    _rebuild_menu_hand();
}

void Control_Scene_Home::_on_sky_time_changed(double value)
{
    // 保留接口，暂无实现
}


void Control_Scene_Home::_on_language_changed()
{
    if (hand_state == HAND_MENU)
        _rebuild_menu_hand();
    else if (hand_state == HAND_SETTINGS_MAIN)
        _rebuild_settings_main_hand();
}

// ================================================================
// ── 徽章显示 ────────────────────────────────────────────────────
// ================================================================

void Control_Scene_Home::_update_badge_display()
{
    if (!badge_icon || !settings) return;

    TextureRect* texture_rect = Object::cast_to<TextureRect>(badge_icon);
    if (!texture_rect) return;

    int badge_value = settings->GetValBadge();
    String texture_path;

    switch (badge_value)
    {
        case 0:
            texture_rect->set_texture(Ref<Texture2D>());
            texture_rect->set_visible(false);
            return;
        case 1: texture_path = "res://Media/Image/badge_1.png"; break;
        case 2: texture_path = "res://Media/Image/badge_2.png"; break;
        case 3: texture_path = "res://Media/Image/badge_3.png"; break;
        default:
            UtilityFunctions::printerr("Invalid badge value: ", badge_value);
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
        UtilityFunctions::printerr("Failed to load badge texture: ", texture_path);
        texture_rect->set_visible(false);
    }
}

// ================================================================
// ── Godot 方法绑定 ───────────────────────────────────────────────
// ================================================================

void Control_Scene_Home::_bind_methods()
{
    ClassDB::bind_method(
        D_METHOD("_on_button_pressed", "scene_name"),
        &Control_Scene_Home::_on_button_pressed);

    ClassDB::bind_method(
        D_METHOD("_on_settings_button_pressed"),
        &Control_Scene_Home::_on_settings_button_pressed);

    ClassDB::bind_method(
        D_METHOD("_on_close_button_pressed"),
        &Control_Scene_Home::_on_close_button_pressed);

    ClassDB::bind_method(
        D_METHOD("_on_sky_time_changed", "value"),
        &Control_Scene_Home::_on_sky_time_changed);

    ClassDB::bind_method(
        D_METHOD("_update_badge_display"),
        &Control_Scene_Home::_update_badge_display);

    // 注：_on_setting_enum_changed / _on_setting_bool_toggled / _refresh_settings_ui
    // 已由卡牌系统取代，不再绑定
}