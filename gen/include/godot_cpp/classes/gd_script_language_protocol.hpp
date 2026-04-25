/**************************************************************************/
/*  gd_script_language_protocol.hpp                                       */
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

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/jsonrpc.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <type_traits>

namespace godot {

class Dictionary;
class GDScriptTextDocument;
class GDScriptWorkspace;
class String;

class GDScriptLanguageProtocol : public JSONRPC {
	GDEXTENSION_CLASS(GDScriptLanguageProtocol, JSONRPC)

	static GDScriptLanguageProtocol *singleton;

public:
	static GDScriptLanguageProtocol *get_singleton();

	Ref<GDScriptTextDocument> get_text_document();
	Ref<GDScriptWorkspace> get_workspace();
	bool is_smart_resolve_enabled() const;
	bool is_initialized() const;
	Variant initialize(const Dictionary &p_params);
	void initialized(const Variant &p_params);
	Error on_client_connected();
	void on_client_disconnected(int32_t p_client_id);
	void notify_client(const String &p_method, const Variant &p_params = nullptr, int32_t p_client_id = -1);

protected:
	template <typename T, typename B>
	static void register_virtuals() {
		JSONRPC::register_virtuals<T, B>();
	}

	~GDScriptLanguageProtocol();

public:
};

} // namespace godot

