/**************************************************************************/
/*  rd_pipeline_shader.cpp                                                */
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

#include <godot_cpp/classes/rd_pipeline_shader.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/rd_pipeline_specialization_constant.hpp>

namespace godot {

void RDPipelineShader::set_shader(const RID &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDPipelineShader::get_class_static()._native_ptr(), StringName("set_shader")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

RID RDPipelineShader::get_shader() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDPipelineShader::get_class_static()._native_ptr(), StringName("get_shader")._native_ptr(), 2944877500);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner);
}

void RDPipelineShader::set_specialization_constants(const TypedArray<Ref<RDPipelineSpecializationConstant>> &p_specialization_constants) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDPipelineShader::get_class_static()._native_ptr(), StringName("set_specialization_constants")._native_ptr(), 381264803);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_specialization_constants);
}

TypedArray<Ref<RDPipelineSpecializationConstant>> RDPipelineShader::get_specialization_constants() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDPipelineShader::get_class_static()._native_ptr(), StringName("get_specialization_constants")._native_ptr(), 3995934104);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (TypedArray<Ref<RDPipelineSpecializationConstant>>()));
	return ::godot::internal::_call_native_mb_ret<TypedArray<Ref<RDPipelineSpecializationConstant>>>(_gde_method_bind, _owner);
}

} // namespace godot
