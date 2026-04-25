/**************************************************************************/
/*  rd_acceleration_structure_geometry.hpp                                */
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

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

class RDAccelerationStructureGeometry : public RefCounted {
	GDEXTENSION_CLASS(RDAccelerationStructureGeometry, RefCounted)

public:
	void set_flags(BitField<RenderingDevice::AccelerationStructureGeometryFlagBits> p_member);
	BitField<RenderingDevice::AccelerationStructureGeometryFlagBits> get_flags() const;
	void set_vertex_buffer(const RID &p_member);
	RID get_vertex_buffer() const;
	void set_vertex_offset(uint32_t p_member);
	uint32_t get_vertex_offset() const;
	void set_vertex_stride(uint32_t p_member);
	uint32_t get_vertex_stride() const;
	void set_vertex_count(uint32_t p_member);
	uint32_t get_vertex_count() const;
	void set_vertex_format(RenderingDevice::DataFormat p_member);
	RenderingDevice::DataFormat get_vertex_format() const;
	void set_index_buffer(const RID &p_member);
	RID get_index_buffer() const;
	void set_index_offset(uint32_t p_member);
	uint32_t get_index_offset() const;
	void set_index_count(uint32_t p_member);
	uint32_t get_index_count() const;

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		RefCounted::register_virtuals<T, B>();
	}

public:
};

} // namespace godot

