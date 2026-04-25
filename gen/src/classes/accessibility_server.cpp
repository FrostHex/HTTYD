/**************************************************************************/
/*  accessibility_server.cpp                                              */
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

#include <godot_cpp/classes/accessibility_server.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/transform2d.hpp>

namespace godot {

AccessibilityServer *AccessibilityServer::singleton = nullptr;

AccessibilityServer *AccessibilityServer::get_singleton() {
	if (unlikely(singleton == nullptr)) {
		GDExtensionObjectPtr singleton_obj = ::godot::gdextension_interface::global_get_singleton(AccessibilityServer::get_class_static()._native_ptr());
#ifdef DEBUG_ENABLED
		ERR_FAIL_NULL_V(singleton_obj, nullptr);
#endif // DEBUG_ENABLED
		singleton = reinterpret_cast<AccessibilityServer *>(::godot::gdextension_interface::object_get_instance_binding(singleton_obj, ::godot::gdextension_interface::token, &AccessibilityServer::_gde_binding_callbacks));
#ifdef DEBUG_ENABLED
		ERR_FAIL_NULL_V(singleton, nullptr);
#endif // DEBUG_ENABLED
		if (likely(singleton)) {
			ClassDB::_register_engine_singleton(AccessibilityServer::get_class_static(), singleton);
		}
	}
	return singleton;
}

AccessibilityServer::~AccessibilityServer() {
	if (singleton == this) {
		ClassDB::_unregister_engine_singleton(AccessibilityServer::get_class_static());
		singleton = nullptr;
	}
}

bool AccessibilityServer::is_supported() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("is_supported")._native_ptr(), 36873697);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner);
}

RID AccessibilityServer::create_element(int32_t p_window_id, AccessibilityServer::AccessibilityRole p_role) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("create_element")._native_ptr(), 3846965249);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	int64_t p_window_id_encoded;
	PtrToArg<int64_t>::encode(p_window_id, &p_window_id_encoded);
	int64_t p_role_encoded;
	PtrToArg<int64_t>::encode(p_role, &p_role_encoded);
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner, &p_window_id_encoded, &p_role_encoded);
}

RID AccessibilityServer::create_sub_element(const RID &p_parent_rid, AccessibilityServer::AccessibilityRole p_role, int32_t p_insert_pos) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("create_sub_element")._native_ptr(), 1151690429);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	int64_t p_role_encoded;
	PtrToArg<int64_t>::encode(p_role, &p_role_encoded);
	int64_t p_insert_pos_encoded;
	PtrToArg<int64_t>::encode(p_insert_pos, &p_insert_pos_encoded);
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner, &p_parent_rid, &p_role_encoded, &p_insert_pos_encoded);
}

RID AccessibilityServer::create_sub_text_edit_elements(const RID &p_parent_rid, const RID &p_shaped_text, float p_min_height, int32_t p_insert_pos, bool p_is_last_line) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("create_sub_text_edit_elements")._native_ptr(), 2702009895);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	double p_min_height_encoded;
	PtrToArg<double>::encode(p_min_height, &p_min_height_encoded);
	int64_t p_insert_pos_encoded;
	PtrToArg<int64_t>::encode(p_insert_pos, &p_insert_pos_encoded);
	int8_t p_is_last_line_encoded;
	PtrToArg<bool>::encode(p_is_last_line, &p_is_last_line_encoded);
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner, &p_parent_rid, &p_shaped_text, &p_min_height_encoded, &p_insert_pos_encoded, &p_is_last_line_encoded);
}

bool AccessibilityServer::has_element(const RID &p_id) const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("has_element")._native_ptr(), 4155700596);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner, &p_id);
}

void AccessibilityServer::free_element(const RID &p_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("free_element")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id);
}

void AccessibilityServer::element_set_meta(const RID &p_id, const Variant &p_meta) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("element_set_meta")._native_ptr(), 3175752987);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_meta);
}

Variant AccessibilityServer::element_get_meta(const RID &p_id) const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("element_get_meta")._native_ptr(), 4171304767);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_id);
}

void AccessibilityServer::set_window_rect(int32_t p_window_id, const Rect2 &p_rect_out, const Rect2 &p_rect_in) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("set_window_rect")._native_ptr(), 2386961724);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_window_id_encoded;
	PtrToArg<int64_t>::encode(p_window_id, &p_window_id_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_window_id_encoded, &p_rect_out, &p_rect_in);
}

void AccessibilityServer::set_window_focused(int32_t p_window_id, bool p_focused) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("set_window_focused")._native_ptr(), 300928843);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_window_id_encoded;
	PtrToArg<int64_t>::encode(p_window_id, &p_window_id_encoded);
	int8_t p_focused_encoded;
	PtrToArg<bool>::encode(p_focused, &p_focused_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_window_id_encoded, &p_focused_encoded);
}

void AccessibilityServer::update_set_focus(const RID &p_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_focus")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id);
}

RID AccessibilityServer::get_window_root(int32_t p_window_id) const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("get_window_root")._native_ptr(), 495598643);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	int64_t p_window_id_encoded;
	PtrToArg<int64_t>::encode(p_window_id, &p_window_id_encoded);
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner, &p_window_id_encoded);
}

void AccessibilityServer::update_set_role(const RID &p_id, AccessibilityServer::AccessibilityRole p_role) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_role")._native_ptr(), 3747886520);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_role_encoded;
	PtrToArg<int64_t>::encode(p_role, &p_role_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_role_encoded);
}

void AccessibilityServer::update_set_name(const RID &p_id, const String &p_name) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_name")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_name);
}

void AccessibilityServer::update_set_braille_label(const RID &p_id, const String &p_name) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_braille_label")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_name);
}

void AccessibilityServer::update_set_braille_role_description(const RID &p_id, const String &p_description) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_braille_role_description")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_description);
}

void AccessibilityServer::update_set_extra_info(const RID &p_id, const String &p_name) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_extra_info")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_name);
}

void AccessibilityServer::update_set_description(const RID &p_id, const String &p_description) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_description")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_description);
}

void AccessibilityServer::update_set_value(const RID &p_id, const String &p_value) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_value")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_value);
}

void AccessibilityServer::update_set_tooltip(const RID &p_id, const String &p_tooltip) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_tooltip")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_tooltip);
}

void AccessibilityServer::update_set_bounds(const RID &p_id, const Rect2 &p_rect) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_bounds")._native_ptr(), 1378122625);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_rect);
}

void AccessibilityServer::update_set_transform(const RID &p_id, const Transform2D &p_transform) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_transform")._native_ptr(), 1246044741);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_transform);
}

void AccessibilityServer::update_add_child(const RID &p_id, const RID &p_child_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_child")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_child_id);
}

void AccessibilityServer::update_add_related_controls(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_controls")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_add_related_details(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_details")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_add_related_described_by(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_described_by")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_add_related_flow_to(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_flow_to")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_add_related_labeled_by(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_labeled_by")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_add_related_radio_group(const RID &p_id, const RID &p_related_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_related_radio_group")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_related_id);
}

void AccessibilityServer::update_set_active_descendant(const RID &p_id, const RID &p_other_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_active_descendant")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_other_id);
}

void AccessibilityServer::update_set_next_on_line(const RID &p_id, const RID &p_other_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_next_on_line")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_other_id);
}

void AccessibilityServer::update_set_previous_on_line(const RID &p_id, const RID &p_other_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_previous_on_line")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_other_id);
}

void AccessibilityServer::update_set_member_of(const RID &p_id, const RID &p_group_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_member_of")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_group_id);
}

void AccessibilityServer::update_set_in_page_link_target(const RID &p_id, const RID &p_other_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_in_page_link_target")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_other_id);
}

void AccessibilityServer::update_set_error_message(const RID &p_id, const RID &p_other_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_error_message")._native_ptr(), 395945892);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_other_id);
}

void AccessibilityServer::update_set_live(const RID &p_id, AccessibilityServer::AccessibilityLiveMode p_live) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_live")._native_ptr(), 2993365237);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_live_encoded;
	PtrToArg<int64_t>::encode(p_live, &p_live_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_live_encoded);
}

void AccessibilityServer::update_add_action(const RID &p_id, AccessibilityServer::AccessibilityAction p_action, const Callable &p_callable) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_action")._native_ptr(), 3960092835);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_action_encoded;
	PtrToArg<int64_t>::encode(p_action, &p_action_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_action_encoded, &p_callable);
}

void AccessibilityServer::update_add_custom_action(const RID &p_id, int32_t p_action_id, const String &p_action_description) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_add_custom_action")._native_ptr(), 4153150897);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_action_id_encoded;
	PtrToArg<int64_t>::encode(p_action_id, &p_action_id_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_action_id_encoded, &p_action_description);
}

void AccessibilityServer::update_set_table_row_count(const RID &p_id, int32_t p_count) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_row_count")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_count_encoded;
	PtrToArg<int64_t>::encode(p_count, &p_count_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_count_encoded);
}

void AccessibilityServer::update_set_table_column_count(const RID &p_id, int32_t p_count) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_column_count")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_count_encoded;
	PtrToArg<int64_t>::encode(p_count, &p_count_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_count_encoded);
}

void AccessibilityServer::update_set_table_row_index(const RID &p_id, int32_t p_index) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_row_index")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_index_encoded;
	PtrToArg<int64_t>::encode(p_index, &p_index_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_index_encoded);
}

void AccessibilityServer::update_set_table_column_index(const RID &p_id, int32_t p_index) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_column_index")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_index_encoded;
	PtrToArg<int64_t>::encode(p_index, &p_index_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_index_encoded);
}

void AccessibilityServer::update_set_table_cell_position(const RID &p_id, int32_t p_row_index, int32_t p_column_index) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_cell_position")._native_ptr(), 4288446313);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_row_index_encoded;
	PtrToArg<int64_t>::encode(p_row_index, &p_row_index_encoded);
	int64_t p_column_index_encoded;
	PtrToArg<int64_t>::encode(p_column_index, &p_column_index_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_row_index_encoded, &p_column_index_encoded);
}

void AccessibilityServer::update_set_table_cell_span(const RID &p_id, int32_t p_row_span, int32_t p_column_span) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_table_cell_span")._native_ptr(), 4288446313);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_row_span_encoded;
	PtrToArg<int64_t>::encode(p_row_span, &p_row_span_encoded);
	int64_t p_column_span_encoded;
	PtrToArg<int64_t>::encode(p_column_span, &p_column_span_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_row_span_encoded, &p_column_span_encoded);
}

void AccessibilityServer::update_set_list_item_count(const RID &p_id, int32_t p_size) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_item_count")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_size_encoded;
	PtrToArg<int64_t>::encode(p_size, &p_size_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_size_encoded);
}

void AccessibilityServer::update_set_list_item_index(const RID &p_id, int32_t p_index) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_item_index")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_index_encoded;
	PtrToArg<int64_t>::encode(p_index, &p_index_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_index_encoded);
}

void AccessibilityServer::update_set_list_item_level(const RID &p_id, int32_t p_level) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_item_level")._native_ptr(), 3411492887);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_level_encoded;
	PtrToArg<int64_t>::encode(p_level, &p_level_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_level_encoded);
}

void AccessibilityServer::update_set_list_item_selected(const RID &p_id, bool p_selected) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_item_selected")._native_ptr(), 1265174801);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_selected_encoded;
	PtrToArg<bool>::encode(p_selected, &p_selected_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_selected_encoded);
}

void AccessibilityServer::update_set_list_item_expanded(const RID &p_id, bool p_expanded) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_item_expanded")._native_ptr(), 1265174801);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_expanded_encoded;
	PtrToArg<bool>::encode(p_expanded, &p_expanded_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_expanded_encoded);
}

void AccessibilityServer::update_set_popup_type(const RID &p_id, AccessibilityServer::AccessibilityPopupType p_popup) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_popup_type")._native_ptr(), 690307634);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_popup_encoded;
	PtrToArg<int64_t>::encode(p_popup, &p_popup_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_popup_encoded);
}

void AccessibilityServer::update_set_checked(const RID &p_id, bool p_checekd) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_checked")._native_ptr(), 1265174801);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_checekd_encoded;
	PtrToArg<bool>::encode(p_checekd, &p_checekd_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_checekd_encoded);
}

void AccessibilityServer::update_set_num_value(const RID &p_id, double p_position) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_num_value")._native_ptr(), 1794382983);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_position_encoded;
	PtrToArg<double>::encode(p_position, &p_position_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_position_encoded);
}

void AccessibilityServer::update_set_num_range(const RID &p_id, double p_min, double p_max) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_num_range")._native_ptr(), 2513314492);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_min_encoded;
	PtrToArg<double>::encode(p_min, &p_min_encoded);
	double p_max_encoded;
	PtrToArg<double>::encode(p_max, &p_max_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_min_encoded, &p_max_encoded);
}

void AccessibilityServer::update_set_num_step(const RID &p_id, double p_step) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_num_step")._native_ptr(), 1794382983);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_step_encoded;
	PtrToArg<double>::encode(p_step, &p_step_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_step_encoded);
}

void AccessibilityServer::update_set_num_jump(const RID &p_id, double p_jump) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_num_jump")._native_ptr(), 1794382983);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_jump_encoded;
	PtrToArg<double>::encode(p_jump, &p_jump_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_jump_encoded);
}

void AccessibilityServer::update_set_scroll_x(const RID &p_id, double p_position) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_scroll_x")._native_ptr(), 1794382983);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_position_encoded;
	PtrToArg<double>::encode(p_position, &p_position_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_position_encoded);
}

void AccessibilityServer::update_set_scroll_x_range(const RID &p_id, double p_min, double p_max) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_scroll_x_range")._native_ptr(), 2513314492);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_min_encoded;
	PtrToArg<double>::encode(p_min, &p_min_encoded);
	double p_max_encoded;
	PtrToArg<double>::encode(p_max, &p_max_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_min_encoded, &p_max_encoded);
}

void AccessibilityServer::update_set_scroll_y(const RID &p_id, double p_position) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_scroll_y")._native_ptr(), 1794382983);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_position_encoded;
	PtrToArg<double>::encode(p_position, &p_position_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_position_encoded);
}

void AccessibilityServer::update_set_scroll_y_range(const RID &p_id, double p_min, double p_max) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_scroll_y_range")._native_ptr(), 2513314492);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_min_encoded;
	PtrToArg<double>::encode(p_min, &p_min_encoded);
	double p_max_encoded;
	PtrToArg<double>::encode(p_max, &p_max_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_min_encoded, &p_max_encoded);
}

void AccessibilityServer::update_set_text_decorations(const RID &p_id, bool p_underline, bool p_strikethrough, bool p_overline, const Color &p_color) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_text_decorations")._native_ptr(), 457503484);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_underline_encoded;
	PtrToArg<bool>::encode(p_underline, &p_underline_encoded);
	int8_t p_strikethrough_encoded;
	PtrToArg<bool>::encode(p_strikethrough, &p_strikethrough_encoded);
	int8_t p_overline_encoded;
	PtrToArg<bool>::encode(p_overline, &p_overline_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_underline_encoded, &p_strikethrough_encoded, &p_overline_encoded, &p_color);
}

void AccessibilityServer::update_set_text_align(const RID &p_id, HorizontalAlignment p_align) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_text_align")._native_ptr(), 3725995085);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_align_encoded;
	PtrToArg<int64_t>::encode(p_align, &p_align_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_align_encoded);
}

void AccessibilityServer::update_set_text_selection(const RID &p_id, const RID &p_text_start_id, int32_t p_start_char, const RID &p_text_end_id, int32_t p_end_char) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_text_selection")._native_ptr(), 3119144029);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_start_char_encoded;
	PtrToArg<int64_t>::encode(p_start_char, &p_start_char_encoded);
	int64_t p_end_char_encoded;
	PtrToArg<int64_t>::encode(p_end_char, &p_end_char_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_text_start_id, &p_start_char_encoded, &p_text_end_id, &p_end_char_encoded);
}

void AccessibilityServer::update_set_flag(const RID &p_id, AccessibilityServer::AccessibilityFlags p_flag, bool p_value) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_flag")._native_ptr(), 1473043386);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_flag_encoded;
	PtrToArg<int64_t>::encode(p_flag, &p_flag_encoded);
	int8_t p_value_encoded;
	PtrToArg<bool>::encode(p_value, &p_value_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_flag_encoded, &p_value_encoded);
}

void AccessibilityServer::update_set_classname(const RID &p_id, const String &p_classname) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_classname")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_classname);
}

void AccessibilityServer::update_set_placeholder(const RID &p_id, const String &p_placeholder) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_placeholder")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_placeholder);
}

void AccessibilityServer::update_set_language(const RID &p_id, const String &p_language) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_language")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_language);
}

void AccessibilityServer::update_set_text_orientation(const RID &p_id, bool p_vertical) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_text_orientation")._native_ptr(), 1265174801);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_vertical_encoded;
	PtrToArg<bool>::encode(p_vertical, &p_vertical_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_vertical_encoded);
}

void AccessibilityServer::update_set_list_orientation(const RID &p_id, bool p_vertical) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_list_orientation")._native_ptr(), 1265174801);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_vertical_encoded;
	PtrToArg<bool>::encode(p_vertical, &p_vertical_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_vertical_encoded);
}

void AccessibilityServer::update_set_shortcut(const RID &p_id, const String &p_shortcut) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_shortcut")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_shortcut);
}

void AccessibilityServer::update_set_url(const RID &p_id, const String &p_url) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_url")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_url);
}

void AccessibilityServer::update_set_role_description(const RID &p_id, const String &p_description) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_role_description")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_description);
}

void AccessibilityServer::update_set_state_description(const RID &p_id, const String &p_description) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_state_description")._native_ptr(), 2726140452);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_description);
}

void AccessibilityServer::update_set_color_value(const RID &p_id, const Color &p_color) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_color_value")._native_ptr(), 2948539648);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_color);
}

void AccessibilityServer::update_set_background_color(const RID &p_id, const Color &p_color) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_background_color")._native_ptr(), 2948539648);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_color);
}

void AccessibilityServer::update_set_foreground_color(const RID &p_id, const Color &p_color) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(AccessibilityServer::get_class_static()._native_ptr(), StringName("update_set_foreground_color")._native_ptr(), 2948539648);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_id, &p_color);
}

} // namespace godot
