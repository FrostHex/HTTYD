/**************************************************************************/
/*  virtual_joystick.cpp                                                  */
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

#include <godot_cpp/classes/virtual_joystick.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

namespace godot {

void VirtualJoystick::set_joystick_mode(VirtualJoystick::JoystickMode p_mode) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_joystick_mode")._native_ptr(), 1316760817);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_mode_encoded;
	PtrToArg<int64_t>::encode(p_mode, &p_mode_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_mode_encoded);
}

VirtualJoystick::JoystickMode VirtualJoystick::get_joystick_mode() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_joystick_mode")._native_ptr(), 2694680530);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (VirtualJoystick::JoystickMode(0)));
	return (VirtualJoystick::JoystickMode)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_joystick_size(float p_size) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_joystick_size")._native_ptr(), 373806689);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_size_encoded;
	PtrToArg<double>::encode(p_size, &p_size_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_size_encoded);
}

float VirtualJoystick::get_joystick_size() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_joystick_size")._native_ptr(), 1740695150);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0.0));
	return ::godot::internal::_call_native_mb_ret<double>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_tip_size(float p_size) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_tip_size")._native_ptr(), 373806689);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_size_encoded;
	PtrToArg<double>::encode(p_size, &p_size_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_size_encoded);
}

float VirtualJoystick::get_tip_size() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_tip_size")._native_ptr(), 1740695150);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0.0));
	return ::godot::internal::_call_native_mb_ret<double>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_deadzone_ratio(float p_ratio) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_deadzone_ratio")._native_ptr(), 373806689);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_ratio_encoded;
	PtrToArg<double>::encode(p_ratio, &p_ratio_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_ratio_encoded);
}

float VirtualJoystick::get_deadzone_ratio() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_deadzone_ratio")._native_ptr(), 1740695150);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0.0));
	return ::godot::internal::_call_native_mb_ret<double>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_clampzone_ratio(float p_ratio) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_clampzone_ratio")._native_ptr(), 373806689);
	CHECK_METHOD_BIND(_gde_method_bind);
	double p_ratio_encoded;
	PtrToArg<double>::encode(p_ratio, &p_ratio_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_ratio_encoded);
}

float VirtualJoystick::get_clampzone_ratio() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_clampzone_ratio")._native_ptr(), 1740695150);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0.0));
	return ::godot::internal::_call_native_mb_ret<double>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_initial_offset_ratio(const Vector2 &p_ratio) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_initial_offset_ratio")._native_ptr(), 743155724);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_ratio);
}

Vector2 VirtualJoystick::get_initial_offset_ratio() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_initial_offset_ratio")._native_ptr(), 3341600327);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Vector2()));
	return ::godot::internal::_call_native_mb_ret<Vector2>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_action_left(const StringName &p_action) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_action_left")._native_ptr(), 3304788590);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_action);
}

StringName VirtualJoystick::get_action_left() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_action_left")._native_ptr(), 2002593661);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (StringName()));
	return ::godot::internal::_call_native_mb_ret<StringName>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_action_right(const StringName &p_action) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_action_right")._native_ptr(), 3304788590);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_action);
}

StringName VirtualJoystick::get_action_right() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_action_right")._native_ptr(), 2002593661);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (StringName()));
	return ::godot::internal::_call_native_mb_ret<StringName>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_action_up(const StringName &p_action) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_action_up")._native_ptr(), 3304788590);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_action);
}

StringName VirtualJoystick::get_action_up() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_action_up")._native_ptr(), 2002593661);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (StringName()));
	return ::godot::internal::_call_native_mb_ret<StringName>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_action_down(const StringName &p_action) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_action_down")._native_ptr(), 3304788590);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_action);
}

StringName VirtualJoystick::get_action_down() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_action_down")._native_ptr(), 2002593661);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (StringName()));
	return ::godot::internal::_call_native_mb_ret<StringName>(_gde_method_bind, _owner);
}

void VirtualJoystick::set_visibility_mode(VirtualJoystick::VisibilityMode p_mode) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("set_visibility_mode")._native_ptr(), 2638298545);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_mode_encoded;
	PtrToArg<int64_t>::encode(p_mode, &p_mode_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_mode_encoded);
}

VirtualJoystick::VisibilityMode VirtualJoystick::get_visibility_mode() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(VirtualJoystick::get_class_static()._native_ptr(), StringName("get_visibility_mode")._native_ptr(), 3530872950);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (VirtualJoystick::VisibilityMode(0)));
	return (VirtualJoystick::VisibilityMode)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

} // namespace godot
