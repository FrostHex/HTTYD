// ==================== Menu.cpp ====================
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

#include "Menu.h"
#include "Settings.h"
#include "Control_Main.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/system_font.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/variant/color.hpp>
#include <cmath>

using namespace godot;

// ================================================================
// 构造 / 析构
// ================================================================

Menu::Menu()
{
}

Menu::~Menu()
{
}

// ================================================================
// Initialize
// ================================================================

void Menu::Initialize(
	Node* owner,
	Settings* settings,
	Control_Main* control_main,
	Node* viewport_container)
{
	if (owner && get_parent() != owner)
	{
		owner->add_child(this);
	}

	set_process(true);
	set_process_input(true);

	this->settings = settings;
	this->control_main = control_main;

	if (!viewport_container)
	{
		UtilityFunctions::printerr("Menu: viewport_container is null");
		return;
	}

	Node *ui_root_node = viewport_container->get_node_or_null(
		NodePath("Viewport/CanvasLayer/Control"));
	ui_root = Object::cast_to<Control>(ui_root_node);
	if (!ui_root)
	{
		UtilityFunctions::printerr(
			"Menu: Could not find UI root at Viewport/CanvasLayer/Control");
		return;
	}

	// settings_panel 仍然作为整体面板的 visible 开关
	settings_panel = ui_root->get_node_or_null(NodePath("Settings_Panel"));
	if (!settings_panel)
	{
		UtilityFunctions::printerr(
			"Menu: Could not find Settings_Panel under UI root");
	}

	_build_card_hand_root();
	_rebuild_menu_hand();
}

// ================================================================
// Process — 每帧插值动画
// ================================================================

void Menu::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) return;
	_animate_hand(delta);
}

void Menu::_input(const Ref<InputEvent>& event)
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

void Menu::_bind_methods()
{
}

// ================================================================
// ── 卡牌手牌构建 ────────────────────────────────────────────────
// ================================================================

// ── 创建手牌根节点（锚定到底部中央）────────────────────────────

void Menu::_build_card_hand_root()
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

void Menu::_set_hand_host(Node* host)
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

Vector2 Menu::_get_hand_local_mouse_pos() const
{
	Control* root = Object::cast_to<Control>(card_hand_root);
	if (!root) return Vector2();

	return root->get_local_mouse_position();
}

// ── 重建主菜单手牌 ───────────────────────────────────────────────

void Menu::_rebuild_menu_hand()
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
		{"Menu_Settings", "header_settings", "Settings", "", true,  false},
		{"Menu_Tutorial", "header_tutorial", "Note Book", "Scene_Tutorial", false, false},
		{"Menu_Practice", "header_practice", "Flight Practice", "Scene_Practice", false, false},
		{"Menu_TD",       "header_td",       "Test Drive", "Scene_TD", false, true},
		{"Menu_Dodge",    "header_dodge",    "Dodge", "Scene_Dodge", false, false},
	};

	for (const MenuEntry& entry : entries)
	{
		CardData data;
		data.type          = CARD_MENU;
		data.prop_name     = entry.id;
		String member_name = String(entry.key);
		if (member_name.begins_with("header_"))
			member_name = member_name.substr(7);
		data.member_name   = member_name;
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

void Menu::_rebuild_settings_main_hand()
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
		back.display_name = _get_json_text("header_back", "Back");
		back.value_text   = _get_json_text("header_back", "Back");
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
		String member_name = String(entry.get("member", ""));
		if (prop_name.is_empty()) continue;

		CardData data;
		data.type         = CARD_SETTING;
		data.prop_name    = prop_name;
		data.member_name  = member_name;
		String header_key = member_name.is_empty() ? prop_name : member_name;
		data.display_name = _get_json_text("header_" + header_key, prop_name);
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

void Menu::_rebuild_sub_hand(
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
		back.member_name  = parent_data.member_name;
		back.display_name = _get_json_text("header_back", "Back");
		back.value_text   = _get_json_text("header_back", "Back");
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
	// display_name 在此重新查 JSON，不沿用 parent_data 里的缓存字符串，
	// 确保切换语言后进入子手牌时标题是当前语言。
	String header_key = parent_data.member_name.is_empty() ? prop_name : parent_data.member_name;
	String sub_display_name = _get_json_text("header_" + header_key, prop_name);

	if (parent_data.hint == PROPERTY_HINT_ENUM)
	{
		// enum：按 hint_string 中的逗号分割选项
		PackedStringArray items = parent_data.hint_string.split(",", false);
		for (int idx = 0; idx < items.size(); idx++)
		{
			CardData opt;
			opt.type          = CARD_OPTION;
			opt.prop_name     = prop_name;
			opt.member_name   = parent_data.member_name;
			opt.display_name  = sub_display_name;
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
			opt.member_name   = parent_data.member_name;
			opt.display_name  = sub_display_name;
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

Menu::CardNode
Menu::_create_card_node(const CardData& data, int index)
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

	// ── 卡牌底板：使用图片帧 ─────────────────────────────────
	// 用 TextureRect 铺满卡牌区域，加载 card_frame.png
	TextureRect* frame = memnew(TextureRect);
	frame->set_name("CardFrame");
	frame->set_size(Vector2(CARD_WIDTH, CARD_HEIGHT));
	frame->set_position(Vector2(0.f, 0.f));
	frame->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	frame->set_stretch_mode(TextureRect::STRETCH_SCALE);
	// shaded：卡牌整体略微压暗，产生阴影质感
	frame->set_modulate(Color(0.82f, 0.82f, 0.82f, 1.f));
	{
		Ref<Texture2D> frame_tex = ResourceLoader::get_singleton()->load(
			"res://Media/Image/card_frame.png");
		if (frame_tex.is_valid())
			frame->set_texture(frame_tex);
		else
			UtilityFunctions::printerr("Menu: Failed to load card_frame.png");
	}
	root->add_child(frame);
	// Panel 字段保留兼容性，指向一个透明 Panel（不绘制任何样式）
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

	// ── 顶部标题 Label ───────────────────────────────────────
	// x=0、width=CARD_WIDTH：Label 与卡牌等宽，居中对齐严格以卡牌中轴为基准。
	// OVERRUN_NO_TRIMMING：文字超出 Label 宽度时保持居中溢出，不截断成左对齐。
	Label* title = memnew(Label);
	title->set_name("TitleLabel");
	title->set_text(data.display_name);
	title->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	title->set_text_overrun_behavior(TextServer::OVERRUN_NO_TRIMMING);
	title->add_theme_font_size_override("font_size", TITLE_FONT_SIZE);
	title->add_theme_color_override("font_color", dark_gray);
	if (debug_ui)
		title->add_theme_color_override("font_outline_color", Color(0.f, 0.8f, 0.f, 0.5f));
	root->add_child(title);
	title->set_position(Vector2(0.f, TITLE_Y));
	title->set_size(Vector2(CARD_WIDTH, TITLE_H));
	cn.title_label = title;

	// ── 中央值 Label ─────────────────────────────────────────
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

	// ── 中央值图片 ─────────────────────────────────────────
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

	// ── Bool widget image ─────────────────────────────────
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

	// ── 具体描述 Label ───────────────────────────────────────
	// 从 JSON 读取 "detail_<member>" 键，未找到则显示空字符串
	// 菜单卡牌的 prop_name 格式为 "Menu_Xxx"，去掉 "menu_" 前缀后与 JSON key 匹配
	// 设置卡牌使用 Settings 的 member_name（snake_case）
	{
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
		String detail_text = _get_json_text("detail_" + key, "");

		// x=0、width=CARD_WIDTH：与卡牌等宽，居中对齐严格以卡牌中轴为基准。
		// set_size 在 add_child 之后调用，防止布局系统将其重置。
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
		detail->set_position(Vector2(DETAIL_W_MARGIN * 0.5f, CARD_HEIGHT * DETAIL_Y_RATIO));
		detail->set_size(Vector2(CARD_WIDTH - DETAIL_W_MARGIN, CARD_HEIGHT * DETAIL_H_RATIO));
		cn.detail_label = detail;
	}

	// ── 类型标识（右上角角标） ───────────────────────────────
	Label* type = memnew(Label);
	type->set_name("TypeLabel");
	// 注意：直接传 const char* 给 godot::String 会按 Latin-1 解析，
	// 多字节 UTF-8 字符（汉字、特殊符号）必须用 String::utf8() 显式构造。
	String type_char = String::utf8("?");
	if (data.type == CARD_BACK)                  type_char = String::utf8("◀");
	else if (data.type == CARD_OPTION)           type_char = String::utf8("√");
	else if (data.type == CARD_MENU)             type_char = String::utf8("M");
	else if (data.variant_type == Variant::BOOL) type_char = String::utf8("B");
	else                                         type_char = String::utf8("E");
	type->set_text(type_char);
	type->set_position(Vector2(TYPE_LABEL_X, TYPE_LABEL_Y));
	type->set_size(Vector2(TYPE_LABEL_W, TYPE_LABEL_H));
	type->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	{
		Ref<SystemFont> type_font;
		type_font.instantiate();
		PackedStringArray font_names;
		font_names.push_back("Segoe UI");
		font_names.push_back("Arial");
		type_font->set_font_names(font_names);
		type_font->set_font_weight(700);
		type_font->set_allow_system_fallback(true);
		type->add_theme_font_override("font", type_font);
	}
	type->add_theme_font_size_override("font_size", TYPE_LABEL_FONT_SIZE);
	type->add_theme_color_override("font_color", Color(0.1f, 0.2125f, 0.25f, 1.f));
	root->add_child(type);

	// ── Debug 染色：为各区域叠加半透明色块 ───────────────────
	if (debug_ui)
	{
		auto make_debug_overlay = [&](Vector2 pos, Vector2 size, Color col)
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

		// 顶部标题区域 — 绿色
		make_debug_overlay(
			Vector2(TITLE_X, TITLE_Y),
			Vector2(CARD_WIDTH - TITLE_W_MARGIN, TITLE_H),
			Color(0.f, 1.f, 0.f, 0.25f));

		// 中央值区域 — 蓝色
		make_debug_overlay(
			Vector2(VALUE_X, CARD_HEIGHT * VALUE_Y_RATIO),
			Vector2(CARD_WIDTH - VALUE_W_MARGIN, CARD_HEIGHT * VALUE_H_RATIO),
			Color(0.f, 0.4f, 1.f, 0.25f));

		// Bool widget area - cyan
		make_debug_overlay(
			Vector2(WIDGET_X, CARD_HEIGHT * WIDGET_Y_RATIO),
			Vector2(WIDGET_W, WIDGET_H),
			Color(0.f, 0.75f, 0.8f, 0.25f));

		// 具体描述区域 — 橙色
		make_debug_overlay(
			Vector2(DETAIL_X, CARD_HEIGHT * DETAIL_Y_RATIO),
			Vector2(CARD_WIDTH - DETAIL_W_MARGIN, CARD_HEIGHT * DETAIL_H_RATIO),
			Color(1.f, 0.5f, 0.f, 0.25f));

		// 类型标识区域 — 红色
		make_debug_overlay(
			Vector2(TYPE_LABEL_X, TYPE_LABEL_Y),
			Vector2(TYPE_LABEL_W, TYPE_LABEL_H),
			Color(1.f, 0.f, 0.f, 0.35f));
	}

	if (data.shows_badge)
	{
		TextureRect* badge = memnew(TextureRect);
		float badge_size = 25.f;
		badge->set_name("Badge_Icon");
		badge->set_position(Vector2(7.f, 8.f));
		badge->set_size(Vector2(badge_size, badge_size));
		badge->set_expand_mode(TextureRect::EXPAND_FIT_WIDTH_PROPORTIONAL);
		badge->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
		badge->set_texture_filter(CanvasItem::TEXTURE_FILTER_LINEAR);
		badge->set_visible(false);
		root->add_child(badge);
		badge_icon = badge;
	}

	_update_value_display(cn);

	return cn;
}

// ── 销毁所有手牌节点 ─────────────────────────────────────────────

void Menu::_destroy_hand()
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

void Menu::_layout_hand()
{
	int n = hand_cards.size();
	if (n == 0) return;

	// 扇形角度范围：由 FAN_MAX_ANGLE_DEG 定义
	// 当卡牌数量较少时，限制总角度避免间距过大
	float total_angle = FAN_MAX_ANGLE_DEG;
	if (n < 5)
	{
		// 5张牌时用完整角度，4张及以下逐步缩小，最大间距 = CARD_WIDTH + 20
		total_angle = FAN_MAX_ANGLE_DEG * (static_cast<float>(n) - 1.f) / 4.f;
	}
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

void Menu::_animate_hand(double delta)
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

void Menu::_on_card_used(int index)
{
	if (index < 0 || index >= hand_cards.size()) return;

	// 值拷贝：后续任何 _rebuild_* 都会调用 _destroy_hand() → hand_cards.clear()，
	// 若持有引用则立即悬空。必须在此处拷贝。
	const CardData data = hand_cards[index].data;

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

void Menu::_apply_bool_toggle(const CardData& data)
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

void Menu::_apply_option_select(const CardData& data)
{
	if (!settings) return;

	settings->call(data.prop_name + String("_setter"), data.option_value);

	if (data.prop_name == "Language")
		_on_language_changed();
}

// ── 刷新主手牌所有卡牌的中央值文字 ───────────────────────────────

void Menu::_update_settings_main_value_labels()
{
	if (hand_state != HAND_SETTINGS_MAIN) return;

	for (int i = 0; i < hand_cards.size(); i++)
	{
		CardNode& cn = hand_cards.write[i];
		if (!cn.value_label) continue;
		if (cn.data.type == CARD_BACK) continue;

		String new_val = _format_setting_value(cn.data);
		cn.data.value_text = new_val;
		_update_value_display(cn);
	}
}

// 读取设置项当前值并格式化为字符串
String Menu::_format_setting_value(const CardData& data) const
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

String Menu::_get_value_image_path(const CardData& data) const
{
	const String base_path = "res://Media/Image/";

	if (data.type == CARD_BACK)
		return base_path + String("card_back.png");

	if (data.type == CARD_MENU && !data.member_name.is_empty())
		return base_path + String("card_") + data.member_name + String(".png");

	if (data.variant_type == Variant::BOOL && !data.member_name.is_empty())
		return base_path + String("card_") + data.member_name + String(".png");

	const bool is_language = (data.prop_name == "Language" || data.member_name == "language");
	if (is_language)
	{
		int lang_value = -1;
		if (data.type == CARD_OPTION)
			lang_value = data.option_value;
		else if (settings)
			lang_value = settings->GetValLanguage();

		if (lang_value >= 0)
			return (lang_value == 1)
				? base_path + String("card_zh.png")
				: base_path + String("card_en.png");
	}

	return "";
}

void Menu::_update_value_display(CardNode& cn)
{
	if (cn.value_label)
		cn.value_label->set_text(cn.data.value_text);

	bool showing_value_image = false;
	if (cn.value_image)
	{
		String image_path = _get_value_image_path(cn.data);
		if (!image_path.is_empty() && FileAccess::file_exists(image_path))
		{
			Ref<Texture2D> texture = ResourceLoader::get_singleton()->load(image_path);
			if (texture.is_valid())
			{
				cn.value_image->set_texture(texture);
				cn.value_image->set_visible(true);
				showing_value_image = true;
			}
		}

		if (!showing_value_image)
		{
			cn.value_image->set_texture(Ref<Texture2D>());
			cn.value_image->set_visible(false);
		}
	}

	if (cn.value_label)
		cn.value_label->set_visible(!showing_value_image);

	if (cn.widget_image)
	{
		String widget_path;
		if (cn.data.variant_type == Variant::BOOL && settings)
		{
			bool current = static_cast<bool>(
				settings->call(cn.data.prop_name + String("_getter")));
			widget_path = current
				? String("res://Media/Image/card_true.png")
				: String("res://Media/Image/card_false.png");
		}

		if (!widget_path.is_empty() && FileAccess::file_exists(widget_path))
		{
			Ref<Texture2D> widget_tex = ResourceLoader::get_singleton()->load(widget_path);
			if (widget_tex.is_valid())
			{
				cn.widget_image->set_texture(widget_tex);
				cn.widget_image->set_visible(true);
				return;
			}
		}

		cn.widget_image->set_texture(Ref<Texture2D>());
		cn.widget_image->set_visible(false);
	}
}

// ================================================================
// ── 读取多语言 JSON 文本 ─────────────────────────────────────────
// ================================================================

String Menu::_get_json_text(
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

void Menu::_on_button_pressed(const String& scene_name)
{
	if (control_main)
	{
		// UtilityFunctions::print("Switching to scene: ", scene_name);
		control_main->Switch_Scene(scene_name);
	}
	else
	{
		UtilityFunctions::printerr("Control_Main not available for scene switch");
	}
}

void Menu::_on_settings_button_pressed()
{
	if (settings_panel)
	{
		settings_panel->set("visible", true);
		// 面板打开时重建主手牌，确保显示最新值
		_set_hand_host(settings_panel);
		_rebuild_settings_main_hand();
	}
}

void Menu::_on_close_button_pressed()
{
	if (settings_panel)
	{
		settings_panel->set("visible", false);
	}

	_set_hand_host(ui_root);
	_rebuild_menu_hand();
}

void Menu::_on_language_changed()
{
	if (hand_state == HAND_MENU)
		_rebuild_menu_hand();
	else if (hand_state == HAND_SETTINGS_MAIN)
		_rebuild_settings_main_hand();
	else if (hand_state == HAND_SETTINGS_SUB)
	{
		// 子手牌里切换语言（理论上只有 Language 本身是 enum，不会在子手牌触发，
		// 但为健壮性起见：重建子手牌，prop_name 从 sub_parent_prop 恢复）
		// 需要从 current_settings_list 找回该项的 CardData
		for (int i = 0; i < current_settings_list.size(); i++)
		{
			Dictionary entry = current_settings_list[i];
			if (String(entry.get("prop_name", "")) == sub_parent_prop)
			{
				String member_name = String(entry.get("member", ""));
				CardData parent;
				parent.prop_name    = sub_parent_prop;
				parent.member_name  = member_name;
				String header_key = member_name.is_empty() ? sub_parent_prop : member_name;
				parent.display_name = _get_json_text("header_" + header_key, sub_parent_prop);
				parent.variant_type = static_cast<int>(entry.get("variant_type", Variant::NIL));
				parent.hint         = static_cast<int>(entry.get("hint", PROPERTY_HINT_NONE));
				parent.hint_string  = String(entry.get("hint_string", ""));
				parent.is_custom    = static_cast<bool>(entry.get("is_custom", false));
				_rebuild_sub_hand(sub_parent_prop, parent);
				return;
			}
		}
	}
}

// ================================================================
// ── 徽章显示 ────────────────────────────────────────────────────
// ================================================================

void Menu::_update_badge_display()
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