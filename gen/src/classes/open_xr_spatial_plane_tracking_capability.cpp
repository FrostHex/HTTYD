/**************************************************************************/
/*  open_xr_spatial_plane_tracking_capability.cpp                         */
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

#include <godot_cpp/classes/open_xr_spatial_plane_tracking_capability.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/classes/open_xr_future_result.hpp>
#include <godot_cpp/classes/open_xr_spatial_component_data.hpp>
#include <godot_cpp/variant/rid.hpp>

namespace godot {

bool OpenXRSpatialPlaneTrackingCapability::is_supported() {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(OpenXRSpatialPlaneTrackingCapability::get_class_static()._native_ptr(), StringName("is_supported")._native_ptr(), 2240911060);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner);
}

Ref<OpenXRFutureResult> OpenXRSpatialPlaneTrackingCapability::start_entity_discovery(const RID &p_spatial_context, const TypedArray<Ref<OpenXRSpatialComponentData>> &p_component_data, const Ref<OpenXRStructureBase> &p_next_snapshot_create, const Ref<OpenXRStructureBase> &p_next_snapshot_query, const Callable &p_user_callback) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(OpenXRSpatialPlaneTrackingCapability::get_class_static()._native_ptr(), StringName("start_entity_discovery")._native_ptr(), 3452714169);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (Ref<OpenXRFutureResult>()));
	return Ref<OpenXRFutureResult>::_gde_internal_constructor(::godot::internal::_call_native_mb_ret_obj<OpenXRFutureResult>(_gde_method_bind, _owner, &p_spatial_context, &p_component_data, (p_next_snapshot_create != nullptr ? &p_next_snapshot_create->_owner : nullptr), (p_next_snapshot_query != nullptr ? &p_next_snapshot_query->_owner : nullptr), &p_user_callback));
}

} // namespace godot
