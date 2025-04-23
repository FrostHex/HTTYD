#include "simple_input.h"

#include <godot_cpp/classes/input.hpp>  // 访问输入设备
#include <godot_cpp/core/class_db.hpp>  // 类注册
#include <godot_cpp/variant/utility_functions.hpp> // 打印日志
#include <godot_cpp/godot.hpp>

using namespace godot;

SimpleInput::SimpleInput() {
    // 构造器，可以初始化内容
}

SimpleInput::~SimpleInput() {
    // 析构器
}

void SimpleInput::_bind_methods() {
    // 可以在这里绑定Godot暴露的函数，目前不需要
}

void SimpleInput::_process(double delta) {
    if (Input::get_singleton()->is_action_just_pressed("ui_accept")) {
        UtilityFunctions::print("Spacebar pressed!"); 
    }
}

void initialize_simple_input(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    ClassDB::register_class<SimpleInput>();
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
