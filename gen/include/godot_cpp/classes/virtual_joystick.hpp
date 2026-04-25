/**************************************************************************/
/*  virtual_joystick.hpp                                                  */
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

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

class VirtualJoystick : public Control {
	GDEXTENSION_CLASS(VirtualJoystick, Control)

public:
	enum JoystickMode {
		JOYSTICK_FIXED = 0,
		JOYSTICK_DYNAMIC = 1,
		JOYSTICK_FOLLOWING = 2,
	};

	enum VisibilityMode {
		VISIBILITY_ALWAYS = 0,
		VISIBILITY_WHEN_TOUCHED = 1,
	};

	void set_joystick_mode(VirtualJoystick::JoystickMode p_mode);
	VirtualJoystick::JoystickMode get_joystick_mode() const;
	void set_joystick_size(float p_size);
	float get_joystick_size() const;
	void set_tip_size(float p_size);
	float get_tip_size() const;
	void set_deadzone_ratio(float p_ratio);
	float get_deadzone_ratio() const;
	void set_clampzone_ratio(float p_ratio);
	float get_clampzone_ratio() const;
	void set_initial_offset_ratio(const Vector2 &p_ratio);
	Vector2 get_initial_offset_ratio() const;
	void set_action_left(const StringName &p_action);
	StringName get_action_left() const;
	void set_action_right(const StringName &p_action);
	StringName get_action_right() const;
	void set_action_up(const StringName &p_action);
	StringName get_action_up() const;
	void set_action_down(const StringName &p_action);
	StringName get_action_down() const;
	void set_visibility_mode(VirtualJoystick::VisibilityMode p_mode);
	VirtualJoystick::VisibilityMode get_visibility_mode() const;

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		Control::register_virtuals<T, B>();
	}

public:
};

} // namespace godot

VARIANT_ENUM_CAST(VirtualJoystick::JoystickMode);
VARIANT_ENUM_CAST(VirtualJoystick::VisibilityMode);

