/**************************************************************************/
/*  drawable_texture2d.cpp                                                */
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

#include <godot_cpp/classes/drawable_texture2d.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/engine_ptrcall.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/rect2i.hpp>

namespace godot {

void DrawableTexture2D::set_format(DrawableTexture2D::DrawableFormat p_format) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("set_format")._native_ptr(), 2875673594);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_format_encoded;
	PtrToArg<int64_t>::encode(p_format, &p_format_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_format_encoded);
}

void DrawableTexture2D::set_use_mipmaps(bool p_mipmaps) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("set_use_mipmaps")._native_ptr(), 2586408642);
	CHECK_METHOD_BIND(_gde_method_bind);
	int8_t p_mipmaps_encoded;
	PtrToArg<bool>::encode(p_mipmaps, &p_mipmaps_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_mipmaps_encoded);
}

bool DrawableTexture2D::get_use_mipmaps() const {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("get_use_mipmaps")._native_ptr(), 36873697);
	CHECK_METHOD_BIND_RET(_gde_method_bind, (false));
	return ::godot::internal::_call_native_mb_ret<int8_t>(_gde_method_bind, _owner);
}

void DrawableTexture2D::setup(int32_t p_width, int32_t p_height, DrawableTexture2D::DrawableFormat p_format, const Color &p_color, bool p_use_mipmaps) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("setup")._native_ptr(), 674365339);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_width_encoded;
	PtrToArg<int64_t>::encode(p_width, &p_width_encoded);
	int64_t p_height_encoded;
	PtrToArg<int64_t>::encode(p_height, &p_height_encoded);
	int64_t p_format_encoded;
	PtrToArg<int64_t>::encode(p_format, &p_format_encoded);
	int8_t p_use_mipmaps_encoded;
	PtrToArg<bool>::encode(p_use_mipmaps, &p_use_mipmaps_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_width_encoded, &p_height_encoded, &p_format_encoded, &p_color, &p_use_mipmaps_encoded);
}

void DrawableTexture2D::blit_rect(const Rect2i &p_rect, const Ref<Texture2D> &p_source, const Color &p_modulate, int32_t p_mipmap, const Ref<Material> &p_material) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("blit_rect")._native_ptr(), 319217173);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_mipmap_encoded;
	PtrToArg<int64_t>::encode(p_mipmap, &p_mipmap_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_rect, (p_source != nullptr ? &p_source->_owner : nullptr), &p_modulate, &p_mipmap_encoded, (p_material != nullptr ? &p_material->_owner : nullptr));
}

void DrawableTexture2D::blit_rect_multi(const Rect2i &p_rect, const TypedArray<Ref<Texture2D>> &p_sources, const TypedArray<Ref<DrawableTexture2D>> &p_extra_targets, const Color &p_modulate, int32_t p_mipmap, const Ref<Material> &p_material) {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("blit_rect_multi")._native_ptr(), 3074783066);
	CHECK_METHOD_BIND(_gde_method_bind);
	int64_t p_mipmap_encoded;
	PtrToArg<int64_t>::encode(p_mipmap, &p_mipmap_encoded);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner, &p_rect, &p_sources, &p_extra_targets, &p_modulate, &p_mipmap_encoded, (p_material != nullptr ? &p_material->_owner : nullptr));
}

void DrawableTexture2D::generate_mipmaps() {
	static GDExtensionMethodBindPtr _gde_method_bind = ::godot::gdextension_interface::classdb_get_method_bind(DrawableTexture2D::get_class_static()._native_ptr(), StringName("generate_mipmaps")._native_ptr(), 3218959716);
	CHECK_METHOD_BIND(_gde_method_bind);
	::godot::internal::_call_native_mb_no_ret(_gde_method_bind, _owner);
}

} // namespace godot
