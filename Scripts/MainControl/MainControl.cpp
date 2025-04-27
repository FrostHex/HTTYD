#include "MainControl.h"
#include "DragonControlKeyboard.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>


using namespace godot;

/**
 * @brief bind methods and properties to the Godot engine
 * @note if _bind_methods() is empty, it can still work, but the methods cannot be called in GDScript or C# or the Inspector
 * @note call ClassDB::bind_method() to expose methods to Godot in order to be used in GDScript or C#
 * @note the first line is the setter method, the second line is the getter method
 * @note &MainControl::SetValJoystickInput is the method pointer, which points to the actual method
 * @note this enables the method to be called like "obj.SetValJoystickInput(true)" in GDScript or C#
 * @note call ADD_PROPERTY() to register properties to Godot
 * @note the second and third parameters are names of the binded getters and setters
 * @note after adding the property, it can be accessed in the Inspector of Godot Engine
 * @note the displayed name in the Inspector is "Joystick Input" and the type is boolean
 */
void MainControl::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("joystick_input_setter", "value"), &MainControl::SetValJoystickInput);
    ClassDB::bind_method(D_METHOD("joystick_input_getter"), &MainControl::GetValJoystickInput);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Joystick Input"), "joystick_input_setter", "joystick_input_getter");
}

/**
 * @brief constructor
 */
MainControl::MainControl()
{
}

/**
 * @brief destructor
 */
MainControl::~MainControl()
{
}

/**
 * @brief called when the node and its children are initialized
 */
void MainControl::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // 仅在游戏运行时执行 
    {
        return;
    }
    if (!joystick_input) 
    {
        // memnew is "new" in Godot C++, which dynamically allocates memory for the object
        // memnew() creates an instance of DragonControlKeyboard and returns a pointer to it
        DragonControlKeyboard *keyboard = memnew(DragonControlKeyboard);

        Node *dragon_node = get_parent()->get_node<Node>("Dragon"); // 在父节点中查找与自己同级的 Dragon 节点
        if (dragon_node) 
        {
            dragon_node->add_child(keyboard);
        } 
        else 
        {
            UtilityFunctions::printerr("Could not find Dragon node");
        }
    }
}

/**
 * @brief the setter for joystick_input
 * @param val the value to set
 */
void MainControl::SetValJoystickInput(bool val)
{
    joystick_input = val;
}

/**
 * @brief the getter for joystick_input
 * @note the const keyword indicates that this function does not modify the instance variables
 * @return the value of joystick_input
 */
bool MainControl::GetValJoystickInput() const
{
    return joystick_input;
}

/**
 * @brief the entry point of the module
 * @param get_proc_addr function pointer to get the address of a function in the Godot engine
 * @param lib pointer to the library
 * @param init pointer to the initialization structure
 * @note used to indicate:
 * @note 1. in which level the module(the .dll compiled from these codes) will be registered: (e.g. SCENE, EDITOR, etc.)
 * @note 2. which classes will be registered in the Godot engine
 * @note 3. (optional) may use register_terminator() to clean up the module when it is unloaded
 * @note if deleted, these classes will not be recognized and cannot be used in Godot Engine
 */
extern "C" GDE_EXPORT GDExtensionBool gdextension_init(GDExtensionInterfaceGetProcAddress get_proc_addr,GDExtensionClassLibraryPtr lib,GDExtensionInitialization *init) 
{
    godot::GDExtensionBinding::InitObject obj(get_proc_addr, lib, init);
    obj.register_initializer([](godot::ModuleInitializationLevel lvl) 
    {
        if (lvl == godot::MODULE_INITIALIZATION_LEVEL_SCENE) 
        {
            godot::ClassDB::register_class<MainControl>();
            godot::ClassDB::register_class<DragonControlKeyboard>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}