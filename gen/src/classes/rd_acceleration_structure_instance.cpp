/**************************************************************************/
/*  rd_acceleration_structure_instance.cpp                                */
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

#include <godot_cpp/classes/rd_acceleration_structure_instance.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

namespace godot {

void RDAccelerationStructureInstance::set_transform(const Transform3D &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_transform")._native_ptr(), 2952846383);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

Transform3D RDAccelerationStructureInstance::get_transform() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_transform")._native_ptr(), 3229777777);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Transform3D()));
	return ::godot::internal::_call_native_mb_ret<Transform3D>(_gde_method_bind, _owner);
}

void RDAccelerationStructureInstance::set_id(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_id")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureInstance::get_id() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_id")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureInstance::set_mask(uint8_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_mask")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint8_t RDAccelerationStructureInstance::get_mask() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_mask")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureInstance::set_hit_sbt_range(int64_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_hit_sbt_range")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

int64_t RDAccelerationStructureInstance::get_hit_sbt_range() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_hit_sbt_range")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureInstance::set_flags(BitField<RenderingDevice::AccelerationStructureInstanceFlagBits> p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_flags")._native_ptr(), 2971840141);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

BitField<RenderingDevice::AccelerationStructureInstanceFlagBits> RDAccelerationStructureInstance::get_flags() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_flags")._native_ptr(), 2410182637);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (BitField<RenderingDevice::AccelerationStructureInstanceFlagBits>(0)));
	return (int64_t)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureInstance::set_blas(const RID &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("set_blas")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

RID RDAccelerationStructureInstance::get_blas() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureInstance::get_class_static()._native_ptr(), StringName("get_blas")._native_ptr(), 2944877500);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner);
}

} // namespace godot
