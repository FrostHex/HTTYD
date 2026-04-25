/**************************************************************************/
/*  rd_hit_group.cpp                                                      */
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

#include <godot_cpp/classes/rd_hit_group.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/rd_pipeline_shader.hpp>

namespace godot {

void RDHitGroup::set_closest_hit_shader(const Ref<RDPipelineShader> &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("set_closest_hit_shader")._native_ptr(), 2556777288);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, (p_member != nullptr ? &p_member->_owner : nullptr));
}

Ref<RDPipelineShader> RDHitGroup::get_closest_hit_shader() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("get_closest_hit_shader")._native_ptr(), 2937716847);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<RDPipelineShader>()));
	return Ref<RDPipelineShader>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<RDPipelineShader>(_gde_method_bind, _owner));
}

void RDHitGroup::set_any_hit_shader(const Ref<RDPipelineShader> &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("set_any_hit_shader")._native_ptr(), 2556777288);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, (p_member != nullptr ? &p_member->_owner : nullptr));
}

Ref<RDPipelineShader> RDHitGroup::get_any_hit_shader() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("get_any_hit_shader")._native_ptr(), 2937716847);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<RDPipelineShader>()));
	return Ref<RDPipelineShader>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<RDPipelineShader>(_gde_method_bind, _owner));
}

void RDHitGroup::set_intersection_shader(const Ref<RDPipelineShader> &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("set_intersection_shader")._native_ptr(), 2556777288);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, (p_member != nullptr ? &p_member->_owner : nullptr));
}

Ref<RDPipelineShader> RDHitGroup::get_intersection_shader() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDHitGroup::get_class_static()._native_ptr(), StringName("get_intersection_shader")._native_ptr(), 2937716847);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<RDPipelineShader>()));
	return Ref<RDPipelineShader>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<RDPipelineShader>(_gde_method_bind, _owner));
}

} // namespace godot
