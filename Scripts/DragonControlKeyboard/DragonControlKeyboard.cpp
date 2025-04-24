#include "DragonControlKeyboard.h"
#include "../DragonControlTop/DragonControlTop.h"

#include <godot_cpp/classes/input.hpp>  // 访问输入设备
#include <godot_cpp/classes/input_event.hpp> // 输入事件
#include <godot_cpp/core/class_db.hpp>  // 类注册
#include <godot_cpp/variant/utility_functions.hpp> // 打印日志
#include <godot_cpp/godot.hpp>

using namespace godot;

DragonControlKeyboard::DragonControlKeyboard() {
    input_singleton = Input::get_singleton();
    set_process_input(true);
    set_process(false);
}

DragonControlKeyboard::~DragonControlKeyboard() {
    // 析构器
}

void DragonControlKeyboard::_bind_methods() {
    // No manual binding for _input
}

void DragonControlKeyboard::_process(double delta) {
    // 输入由 _input 处理, 此处保留空实现以减少开销
}

void DragonControlKeyboard::_input(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_accept")) {
        UtilityFunctions::print("Spacebar pressed!");
    }
}

void initialize_simple_input(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    ClassDB::register_class<DragonControlKeyboard>();
    ClassDB::register_class<DragonControlTop>();
}

void uninitialize_simple_input(ModuleInitializationLevel p_level) {
    // Optional cleanup
}

extern "C" GDExtensionBool GDE_EXPORT gdextension_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization
) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_simple_input);
    init_obj.register_terminator(uninitialize_simple_input);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
