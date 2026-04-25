/**************************************************************************/
/*  gd_script_text_document.cpp                                           */
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

#include <godot_cpp/classes/gd_script_text_document.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/string.hpp>

namespace godot {

void GDScriptTextDocument::show_native_symbol_in_editor(const String &p_symbol_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("show_native_symbol_in_editor")._native_ptr(), 83702148);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_symbol_id);
}

void GDScriptTextDocument::didOpen(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("didOpen")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

void GDScriptTextDocument::didClose(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("didClose")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

void GDScriptTextDocument::didChange(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("didChange")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

void GDScriptTextDocument::willSaveWaitUntil(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("willSaveWaitUntil")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

void GDScriptTextDocument::didSave(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("didSave")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

Variant GDScriptTextDocument::nativeSymbol(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("nativeSymbol")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::documentSymbol(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("documentSymbol")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::completion(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("completion")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Dictionary GDScriptTextDocument::resolve(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("resolve")._native_ptr(), 1333564645);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Dictionary()));
	return ::godot::internal::_call_native_mb_ret<Dictionary>(_gde_method_bind, _owner, &p_params);
}

Dictionary GDScriptTextDocument::rename(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("rename")._native_ptr(), 1333564645);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Dictionary()));
	return ::godot::internal::_call_native_mb_ret<Dictionary>(_gde_method_bind, _owner, &p_params);
}

Variant GDScriptTextDocument::prepareRename(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("prepareRename")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::references(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("references")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::foldingRange(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("foldingRange")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::codeLens(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("codeLens")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::documentLink(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("documentLink")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::colorPresentation(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("colorPresentation")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Variant GDScriptTextDocument::hover(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("hover")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

Array GDScriptTextDocument::definition(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("definition")._native_ptr(), 3877611628);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Array()));
	return ::godot::internal::_call_native_mb_ret<Array>(_gde_method_bind, _owner, &p_params);
}

Variant GDScriptTextDocument::declaration(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("declaration")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

Variant GDScriptTextDocument::signatureHelp(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptTextDocument::get_class_static()._native_ptr(), StringName("signatureHelp")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

} // namespace godot
