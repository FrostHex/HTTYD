/**************************************************************************/
/*  rd_acceleration_structure_geometry.cpp                                */
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

#include <godot_cpp/classes/rd_acceleration_structure_geometry.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

namespace godot {

void RDAccelerationStructureGeometry::set_flags(BitField<RenderingDevice::AccelerationStructureGeometryFlagBits> p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_flags")._native_ptr(), 1046628555);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

BitField<RenderingDevice::AccelerationStructureGeometryFlagBits> RDAccelerationStructureGeometry::get_flags() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_flags")._native_ptr(), 1694887119);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (BitField<RenderingDevice::AccelerationStructureGeometryFlagBits>(0)));
	return (int64_t)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_vertex_buffer(const RID &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_vertex_buffer")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

RID RDAccelerationStructureGeometry::get_vertex_buffer() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_vertex_buffer")._native_ptr(), 2944877500);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_vertex_offset(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_vertex_offset")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureGeometry::get_vertex_offset() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_vertex_offset")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_vertex_stride(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_vertex_stride")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureGeometry::get_vertex_stride() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_vertex_stride")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_vertex_count(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_vertex_count")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureGeometry::get_vertex_count() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_vertex_count")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_vertex_format(RenderingDevice::DataFormat p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_vertex_format")._native_ptr(), 565531219);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

RenderingDevice::DataFormat RDAccelerationStructureGeometry::get_vertex_format() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_vertex_format")._native_ptr(), 2235804183);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RenderingDevice::DataFormat(0)));
	return (RenderingDevice::DataFormat)::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_index_buffer(const RID &p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_index_buffer")._native_ptr(), 2722037293);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member);
}

RID RDAccelerationStructureGeometry::get_index_buffer() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_index_buffer")._native_ptr(), 2944877500);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (RID()));
	return ::godot::internal::_call_native_mb_ret<RID>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_index_offset(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_index_offset")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureGeometry::get_index_offset() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_index_offset")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

void RDAccelerationStructureGeometry::set_index_count(uint32_t p_member) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("set_index_count")._native_ptr(), 1286410249);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_member_encoded;
	PtrToArg<int64_t>::encode(p_member, &p_member_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_member_encoded);
}

uint32_t RDAccelerationStructureGeometry::get_index_count() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(RDAccelerationStructureGeometry::get_class_static()._native_ptr(), StringName("get_index_count")._native_ptr(), 3905245786);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (0));
	return ::godot::internal::_call_native_mb_ret<int64_t>(_gde_method_bind, _owner);
}

} // namespace godot
