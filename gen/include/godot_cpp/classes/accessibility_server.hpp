/**************************************************************************/
/*  accessibility_server.hpp                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

// THIS FILE IS GENERATED. EDITS WILL BE LOST.

#pragma once

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

class Callable;
struct Rect2;
class String;
struct Transform2D;

class AccessibilityServer : public Object {
	GDEXTENSION_CLASS(AccessibilityServer, Object)

	static AccessibilityServer *singleton;

public:
	enum AccessibilityRole {
		ROLE_UNKNOWN = 0,
		ROLE_DEFAULT_BUTTON = 1,
		ROLE_AUDIO = 2,
		ROLE_VIDEO = 3,
		ROLE_STATIC_TEXT = 4,
		ROLE_CONTAINER = 5,
		ROLE_PANEL = 6,
		ROLE_BUTTON = 7,
		ROLE_LINK = 8,
		ROLE_CHECK_BOX = 9,
		ROLE_RADIO_BUTTON = 10,
		ROLE_CHECK_BUTTON = 11,
		ROLE_SCROLL_BAR = 12,
		ROLE_SCROLL_VIEW = 13,
		ROLE_SPLITTER = 14,
		ROLE_SLIDER = 15,
		ROLE_SPIN_BUTTON = 16,
		ROLE_PROGRESS_INDICATOR = 17,
		ROLE_TEXT_FIELD = 18,
		ROLE_MULTILINE_TEXT_FIELD = 19,
		ROLE_COLOR_PICKER = 20,
		ROLE_TABLE = 21,
		ROLE_CELL = 22,
		ROLE_ROW = 23,
		ROLE_ROW_GROUP = 24,
		ROLE_ROW_HEADER = 25,
		ROLE_COLUMN_HEADER = 26,
		ROLE_TREE = 27,
		ROLE_TREE_ITEM = 28,
		ROLE_LIST = 29,
		ROLE_LIST_ITEM = 30,
		ROLE_LIST_BOX = 31,
		ROLE_LIST_BOX_OPTION = 32,
		ROLE_TAB_BAR = 33,
		ROLE_TAB = 34,
		ROLE_TAB_PANEL = 35,
		ROLE_MENU_BAR = 36,
		ROLE_MENU = 37,
		ROLE_MENU_ITEM = 38,
		ROLE_MENU_ITEM_CHECK_BOX = 39,
		ROLE_MENU_ITEM_RADIO = 40,
		ROLE_IMAGE = 41,
		ROLE_WINDOW = 42,
		ROLE_TITLE_BAR = 43,
		ROLE_DIALOG = 44,
		ROLE_TOOLTIP = 45,
		ROLE_REGION = 46,
		ROLE_TEXT_RUN = 47,
	};

	enum AccessibilityPopupType {
		POPUP_MENU = 0,
		POPUP_LIST = 1,
		POPUP_TREE = 2,
		POPUP_DIALOG = 3,
	};

	enum AccessibilityFlags {
		FLAG_HIDDEN = 0,
		FLAG_MULTISELECTABLE = 1,
		FLAG_REQUIRED = 2,
		FLAG_VISITED = 3,
		FLAG_BUSY = 4,
		FLAG_MODAL = 5,
		FLAG_TOUCH_PASSTHROUGH = 6,
		FLAG_READONLY = 7,
		FLAG_DISABLED = 8,
		FLAG_CLIPS_CHILDREN = 9,
	};

	enum AccessibilityAction {
		ACTION_CLICK = 0,
		ACTION_FOCUS = 1,
		ACTION_BLUR = 2,
		ACTION_COLLAPSE = 3,
		ACTION_EXPAND = 4,
		ACTION_DECREMENT = 5,
		ACTION_INCREMENT = 6,
		ACTION_HIDE_TOOLTIP = 7,
		ACTION_SHOW_TOOLTIP = 8,
		ACTION_SET_TEXT_SELECTION = 9,
		ACTION_REPLACE_SELECTED_TEXT = 10,
		ACTION_SCROLL_BACKWARD = 11,
		ACTION_SCROLL_DOWN = 12,
		ACTION_SCROLL_FORWARD = 13,
		ACTION_SCROLL_LEFT = 14,
		ACTION_SCROLL_RIGHT = 15,
		ACTION_SCROLL_UP = 16,
		ACTION_SCROLL_INTO_VIEW = 17,
		ACTION_SCROLL_TO_POINT = 18,
		ACTION_SET_SCROLL_OFFSET = 19,
		ACTION_SET_VALUE = 20,
		ACTION_SHOW_CONTEXT_MENU = 21,
		ACTION_CUSTOM = 22,
	};

	enum AccessibilityLiveMode {
		LIVE_OFF = 0,
		LIVE_POLITE = 1,
		LIVE_ASSERTIVE = 2,
	};

	enum AccessibilityScrollUnit {
		SCROLL_UNIT_ITEM = 0,
		SCROLL_UNIT_PAGE = 1,
	};

	enum AccessibilityScrollHint {
		SCROLL_HINT_TOP_LEFT = 0,
		SCROLL_HINT_BOTTOM_RIGHT = 1,
		SCROLL_HINT_TOP_EDGE = 2,
		SCROLL_HINT_BOTTOM_EDGE = 3,
		SCROLL_HINT_LEFT_EDGE = 4,
		SCROLL_HINT_RIGHT_EDGE = 5,
	};

	static AccessibilityServer *get_singleton();

	bool is_supported() const;
	RID create_element(int32_t p_window_id, AccessibilityServer::AccessibilityRole p_role);
	RID create_sub_element(const RID &p_parent_rid, AccessibilityServer::AccessibilityRole p_role, int32_t p_insert_pos = -1);
	RID create_sub_text_edit_elements(const RID &p_parent_rid, const RID &p_shaped_text, float p_min_height, int32_t p_insert_pos = -1, bool p_is_last_line = false);
	bool has_element(const RID &p_id) const;
	void free_element(const RID &p_id);
	void element_set_meta(const RID &p_id, const Variant &p_meta);
	Variant element_get_meta(const RID &p_id) const;
	void set_window_rect(int32_t p_window_id, const Rect2 &p_rect_out, const Rect2 &p_rect_in);
	void set_window_focused(int32_t p_window_id, bool p_focused);
	void update_set_focus(const RID &p_id);
	RID get_window_root(int32_t p_window_id) const;
	void update_set_role(const RID &p_id, AccessibilityServer::AccessibilityRole p_role);
	void update_set_name(const RID &p_id, const String &p_name);
	void update_set_braille_label(const RID &p_id, const String &p_name);
	void update_set_braille_role_description(const RID &p_id, const String &p_description);
	void update_set_extra_info(const RID &p_id, const String &p_name);
	void update_set_description(const RID &p_id, const String &p_description);
	void update_set_value(const RID &p_id, const String &p_value);
	void update_set_tooltip(const RID &p_id, const String &p_tooltip);
	void update_set_bounds(const RID &p_id, const Rect2 &p_rect);
	void update_set_transform(const RID &p_id, const Transform2D &p_transform);
	void update_add_child(const RID &p_id, const RID &p_child_id);
	void update_add_related_controls(const RID &p_id, const RID &p_related_id);
	void update_add_related_details(const RID &p_id, const RID &p_related_id);
	void update_add_related_described_by(const RID &p_id, const RID &p_related_id);
	void update_add_related_flow_to(const RID &p_id, const RID &p_related_id);
	void update_add_related_labeled_by(const RID &p_id, const RID &p_related_id);
	void update_add_related_radio_group(const RID &p_id, const RID &p_related_id);
	void update_set_active_descendant(const RID &p_id, const RID &p_other_id);
	void update_set_next_on_line(const RID &p_id, const RID &p_other_id);
	void update_set_previous_on_line(const RID &p_id, const RID &p_other_id);
	void update_set_member_of(const RID &p_id, const RID &p_group_id);
	void update_set_in_page_link_target(const RID &p_id, const RID &p_other_id);
	void update_set_error_message(const RID &p_id, const RID &p_other_id);
	void update_set_live(const RID &p_id, AccessibilityServer::AccessibilityLiveMode p_live);
	void update_add_action(const RID &p_id, AccessibilityServer::AccessibilityAction p_action, const Callable &p_callable);
	void update_add_custom_action(const RID &p_id, int32_t p_action_id, const String &p_action_description);
	void update_set_table_row_count(const RID &p_id, int32_t p_count);
	void update_set_table_column_count(const RID &p_id, int32_t p_count);
	void update_set_table_row_index(const RID &p_id, int32_t p_index);
	void update_set_table_column_index(const RID &p_id, int32_t p_index);
	void update_set_table_cell_position(const RID &p_id, int32_t p_row_index, int32_t p_column_index);
	void update_set_table_cell_span(const RID &p_id, int32_t p_row_span, int32_t p_column_span);
	void update_set_list_item_count(const RID &p_id, int32_t p_size);
	void update_set_list_item_index(const RID &p_id, int32_t p_index);
	void update_set_list_item_level(const RID &p_id, int32_t p_level);
	void update_set_list_item_selected(const RID &p_id, bool p_selected);
	void update_set_list_item_expanded(const RID &p_id, bool p_expanded);
	void update_set_popup_type(const RID &p_id, AccessibilityServer::AccessibilityPopupType p_popup);
	void update_set_checked(const RID &p_id, bool p_checekd);
	void update_set_num_value(const RID &p_id, double p_position);
	void update_set_num_range(const RID &p_id, double p_min, double p_max);
	void update_set_num_step(const RID &p_id, double p_step);
	void update_set_num_jump(const RID &p_id, double p_jump);
	void update_set_scroll_x(const RID &p_id, double p_position);
	void update_set_scroll_x_range(const RID &p_id, double p_min, double p_max);
	void update_set_scroll_y(const RID &p_id, double p_position);
	void update_set_scroll_y_range(const RID &p_id, double p_min, double p_max);
	void update_set_text_decorations(const RID &p_id, bool p_underline, bool p_strikethrough, bool p_overline, const Color &p_color = Color(0, 0, 0, 1));
	void update_set_text_align(const RID &p_id, HorizontalAlignment p_align);
	void update_set_text_selection(const RID &p_id, const RID &p_text_start_id, int32_t p_start_char, const RID &p_text_end_id, int32_t p_end_char);
	void update_set_flag(const RID &p_id, AccessibilityServer::AccessibilityFlags p_flag, bool p_value);
	void update_set_classname(const RID &p_id, const String &p_classname);
	void update_set_placeholder(const RID &p_id, const String &p_placeholder);
	void update_set_language(const RID &p_id, const String &p_language);
	void update_set_text_orientation(const RID &p_id, bool p_vertical);
	void update_set_list_orientation(const RID &p_id, bool p_vertical);
	void update_set_shortcut(const RID &p_id, const String &p_shortcut);
	void update_set_url(const RID &p_id, const String &p_url);
	void update_set_role_description(const RID &p_id, const String &p_description);
	void update_set_state_description(const RID &p_id, const String &p_description);
	void update_set_color_value(const RID &p_id, const Color &p_color);
	void update_set_background_color(const RID &p_id, const Color &p_color);
	void update_set_foreground_color(const RID &p_id, const Color &p_color);

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		Object::register_virtuals<T, B>();
	}

	~AccessibilityServer();

public:
};

} // namespace godot

VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityRole);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityPopupType);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityFlags);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityAction);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityLiveMode);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityScrollUnit);
VARIANT_ENUM_CAST(AccessibilityServer::AccessibilityScrollHint);

