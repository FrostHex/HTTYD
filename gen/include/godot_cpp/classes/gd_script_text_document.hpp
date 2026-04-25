/**************************************************************************/
/*  gd_script_text_document.hpp                                           */
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
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

class String;

class GDScriptTextDocument : public RefCounted {
	GDEXTENSION_CLASS(GDScriptTextDocument, RefCounted)

public:
	void show_native_symbol_in_editor(const String &p_symbol_id);
	void didOpen(const Variant &p_params);
	void didClose(const Variant &p_params);
	void didChange(const Variant &p_params);
	void willSaveWaitUntil(const Variant &p_params);
	void didSave(const Variant &p_params);
	Variant nativeSymbol(const Dictionary &p_params);
	Array documentSymbol(const Dictionary &p_params);
	Array completion(const Dictionary &p_params);
	Dictionary resolve(const Dictionary &p_params);
	Dictionary rename(const Dictionary &p_params);
	Variant prepareRename(const Dictionary &p_params);
	Array references(const Dictionary &p_params);
	Array foldingRange(const Dictionary &p_params);
	Array codeLens(const Dictionary &p_params);
	Array documentLink(const Dictionary &p_params);
	Array colorPresentation(const Dictionary &p_params);
	Variant hover(const Dictionary &p_params);
	Array definition(const Dictionary &p_params);
	Variant declaration(const Dictionary &p_params);
	Variant signatureHelp(const Dictionary &p_params);

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		RefCounted::register_virtuals<T, B>();
	}

public:
};

} // namespace godot

