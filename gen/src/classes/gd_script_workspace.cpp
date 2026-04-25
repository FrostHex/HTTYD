/**************************************************************************/
/*  gd_script_workspace.cpp                                               */
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

#include <godot_cpp/classes/gd_script_workspace.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace godot {

void GDScriptWorkspace::apply_new_signal(Object *p_obj, const String &p_function, const PackedStringArray &p_args) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("apply_new_signal")._native_ptr(), 3682583557);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, (p_obj != nullptr ? &p_obj->_owner : nullptr), &p_function, &p_args);
}

String GDScriptWorkspace::get_file_path(const String &p_uri) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("get_file_path")._native_ptr(), 1703090593);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (String()));
	return ::godot::internal::_call_native_mb_ret<String>(_gde_method_bind, _owner, &p_uri);
}

String GDScriptWorkspace::get_file_uri(const String &p_path) const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("get_file_uri")._native_ptr(), 3135753539);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (String()));
	return ::godot::internal::_call_native_mb_ret<String>(_gde_method_bind, _owner, &p_path);
}

Dictionary GDScriptWorkspace::generate_script_api(const String &p_path) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("generate_script_api")._native_ptr(), 2786125124);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Dictionary()));
	return ::godot::internal::_call_native_mb_ret<Dictionary>(_gde_method_bind, _owner, &p_path);
}

void GDScriptWorkspace::didDeleteFiles(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("didDeleteFiles")._native_ptr(), 4155329257);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

Error GDScriptWorkspace::parse_script(const String &p_path, const String &p_content) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("parse_script")._native_ptr(), 852856452);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Error(0)));
	return (Error)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner, &p_path, &p_content);
}

Error GDScriptWorkspace::parse_local_script(const String &p_path) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("parse_local_script")._native_ptr(), 166001499);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Error(0)));
	return (Error)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner, &p_path);
}

void GDScriptWorkspace::publish_diagnostics(const String &p_path) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptWorkspace::get_class_static()._native_ptr(), StringName("publish_diagnostics")._native_ptr(), 83702148);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_path);
}

} // namespace godot
