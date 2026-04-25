/**************************************************************************/
/*  gd_script_language_protocol.cpp                                       */
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

#include <godot_cpp/classes/gd_script_language_protocol.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/gd_script_text_document.hpp>
#include <godot_cpp/classes/gd_script_workspace.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

GDScriptLanguageProtocol *GDScriptLanguageProtocol::singleton = nullptr;

GDScriptLanguageProtocol *GDScriptLanguageProtocol::get_singleton() {
	if (unlikely(singleton == nullptr)) {
		GDExtensionObjectPtr singleton_obj = ::godot::gdextension_interface::global_get_singleton(GDScriptLanguageProtocol::get_class_static()._native_ptr());
#ifdef DEBUG_ENABLED
		ERR_FAIL_NULL_V(singleton_obj, nullptr);
#endif // DEBUG_ENABLED
		singleton = reinterpret_cast<GDScriptLanguageProtocol *>(::godot::gdextension_interface::object_get_instance_binding(singleton_obj, ::godot::gdextension_interface::token, &GDScriptLanguageProtocol::_gde_binding_callbacks));
#ifdef DEBUG_ENABLED
		ERR_FAIL_NULL_V(singleton, nullptr);
#endif // DEBUG_ENABLED
		if (likely(singleton)) {
			ClassDB::_register_engine_singleton(GDScriptLanguageProtocol::get_class_static(), singleton);
		}
	}
	return singleton;
}

GDScriptLanguageProtocol::~GDScriptLanguageProtocol() {
	if (singleton == this) {
		ClassDB::_unregister_engine_singleton(GDScriptLanguageProtocol::get_class_static());
		singleton = nullptr;
	}
}

Ref<GDScriptTextDocument> GDScriptLanguageProtocol::get_text_document() {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("get_text_document")._native_ptr(), 770545799);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<GDScriptTextDocument>()));
	return Ref<GDScriptTextDocument>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<GDScriptTextDocument>(_gde_method_bind, _owner));
}

Ref<GDScriptWorkspace> GDScriptLanguageProtocol::get_workspace() {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("get_workspace")._native_ptr(), 969295246);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<GDScriptWorkspace>()));
	return Ref<GDScriptWorkspace>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<GDScriptWorkspace>(_gde_method_bind, _owner));
}

bool GDScriptLanguageProtocol::is_smart_resolve_enabled() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("is_smart_resolve_enabled")._native_ptr(), 36873697);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner);
}

bool GDScriptLanguageProtocol::is_initialized() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("is_initialized")._native_ptr(), 36873697);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner);
}

Variant GDScriptLanguageProtocol::initialize(const Dictionary &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("initialize")._native_ptr(), 3762224011);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Variant()));
	return ::godot::internal::_call_native_mb_ret<Variant>(_gde_method_bind, _owner, &p_params);
}

void GDScriptLanguageProtocol::initialized(const Variant &p_params) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("initialized")._native_ptr(), 1114965689);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_params);
}

Error GDScriptLanguageProtocol::on_client_connected() {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("on_client_connected")._native_ptr(), 166280745);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Error(0)));
	return (Error)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void GDScriptLanguageProtocol::on_client_disconnected(int32_t p_client_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("on_client_disconnected")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_client_id_encoded;
	PtrToArg<int64_t>::encode(p_client_id, &p_client_id_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_client_id_encoded);
}

void GDScriptLanguageProtocol::notify_client(const String &p_method, const Variant &p_params, int32_t p_client_id) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(GDScriptLanguageProtocol::get_class_static()._native_ptr(), StringName("notify_client")._native_ptr(), 2511212011);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_client_id_encoded;
	PtrToArg<int64_t>::encode(p_client_id, &p_client_id_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_method, &p_params, &p_client_id_encoded);
}

} // namespace godot
