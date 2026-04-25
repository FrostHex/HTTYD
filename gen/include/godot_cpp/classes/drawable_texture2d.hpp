/**************************************************************************/
/*  drawable_texture2d.hpp                                                */
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

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

struct Rect2i;

class DrawableTexture2D : public Texture2D {
	GDEXTENSION_CLASS(DrawableTexture2D, Texture2D)

public:
	enum DrawableFormat {
		DRAWABLE_FORMAT_RGBA8 = 0,
		DRAWABLE_FORMAT_RGBA8_SRGB = 1,
		DRAWABLE_FORMAT_RGBAH = 2,
		DRAWABLE_FORMAT_RGBAF = 3,
	};

	void set_format(DrawableTexture2D::DrawableFormat p_format);
	void set_use_mipmaps(bool p_mipmaps);
	bool get_use_mipmaps() const;
	void setup(int32_t p_width, int32_t p_height, DrawableTexture2D::DrawableFormat p_format, const Color &p_color = Color(1, 1, 1, 1), bool p_use_mipmaps = false);
	void blit_rect(const Rect2i &p_rect, const Ref<Texture2D> &p_source, const Color &p_modulate = Color(1, 1, 1, 1), int32_t p_mipmap = 0, const Ref<Material> &p_material = nullptr);
	void blit_rect_multi(const Rect2i &p_rect, const TypedArray<Ref<Texture2D>> &p_sources, const TypedArray<Ref<DrawableTexture2D>> &p_extra_targets, const Color &p_modulate = Color(1, 1, 1, 1), int32_t p_mipmap = 0, const Ref<Material> &p_material = nullptr);
	void generate_mipmaps();

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		Texture2D::register_virtuals<T, B>();
	}

public:
};

} // namespace godot

VARIANT_ENUM_CAST(DrawableTexture2D::DrawableFormat);

