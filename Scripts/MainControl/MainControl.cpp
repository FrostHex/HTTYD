#include "MainControl.h"
#include "DragonControlKeyboard.h"
#include "DragonControlJoystick.h"
#include "DragonAnimator.h"
#include "CameraControl.h"
#include "CheatSheet.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/classes/input_event.hpp> 
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;


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
 * @note 1. when open a scene containing this node in Godot Engine (editor mode, not running)
 * @note 2. when the game starts running and the this node is contained
 */
void MainControl::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        SetValJoystickInput(GetValJoystickInput());
        return;
    }

    timer = memnew(GameTimer);
    add_child(timer);

    Node *dragon_node = get_parent()->get_node<Node>("Dragon");
    CheatSheet *cheat_sheet = dragon_node->get_node<CheatSheet>("CheatSheet");
    DragonControlTop *dragon_control = nullptr;
    CameraControl *camera_ctrl = memnew(CameraControl(sub_view, enable_headset));
    camera_ctrl->set_name("CameraControl"); // set the name of the camera control node
    dragon_node->add_child(camera_ctrl); // add the camera control to the dragon node

    if (enable_headset) 
    {
        // memnew is "new" in Godot C++, which dynamically allocates memory for the object
        // memnew() creates an instance of DragonControlKeyboard and returns a pointer to it
        dragon_control = memnew(DragonControlJoystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control)); // add the dragon control to the dragon node
    }
    else
    {
        dragon_control = memnew(DragonControlKeyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_control));
    }

    camera_ctrl->SetDragonControl(dragon_control); // set the dragon control to the camera control

    // timer list
    // timer->Timer_AddEvent(0.0f, Callable(dragon_control, "set_state").bind(DragonState::STATE_DISABLED));
    timer->Timer_AddEvent(5.0f, Callable(cheat_sheet, "Detatch")); 
    if (enable_headset) 
    {
        // timer->Timer_AddEvent(0.0f, Callable(dragon_control, "set_state").bind(DragonState::STATE_CRISIS));
    }
}


/**
 * @brief called when an input event occurs
 * @param event the input event
 */
void MainControl::_input(const Ref<InputEvent> &event) 
{
    if (event->is_action_pressed("ui_cancel")) // can be customized in Project Settings -> Input Map
    {
        get_tree()->quit(); // Exit the game when Escape key is pressed
    }
}


/**
 * @brief the setter for enable_headset
 * @param val the value to set
 */
void MainControl::SetValJoystickInput(bool val)
{
    enable_headset = val;

    bool current_xr_enabled = ProjectSettings::get_singleton()->get_setting("xr/openxr/enabled");
    if (current_xr_enabled != val) 
    {
        ProjectSettings::get_singleton()->set_setting("xr/openxr/enabled", val);
        Error err = ProjectSettings::get_singleton()->save(); // save the settings to project.godot file
        if (err != OK) 
        {
            UtilityFunctions::printerr("Failed to save project settings: ", err);
        }
    }
}


/**
 * @brief the getter for enable_headset
 * @note the const keyword indicates that this function does not modify the instance variables
 * @return the value of enable_headset
 */
bool MainControl::GetValJoystickInput() const
{
    return enable_headset;
}


/**
 * @brief the setter for sub_view
 * @param val the value to set
 */
void MainControl::SetValSubView(bool val)
{
    sub_view = val;
}


/**
 * @brief the getter for sub_view
 * @note the const keyword indicates that this function does not modify the instance variables
 * @return the value of sub_view
 */
bool MainControl::GetValSubView() const
{
    return sub_view;
}


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
 * @note the displayed name in the Inspector is "Enable Headset" and the type is boolean
 */
void MainControl::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("enable_headset_setter", "value"), &MainControl::SetValJoystickInput);
    ClassDB::bind_method(D_METHOD("enable_headset_getter"), &MainControl::GetValJoystickInput);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Enable Headset"), "enable_headset_setter", "enable_headset_getter");
    ClassDB::bind_method(D_METHOD("sub_view_setter", "value"), &MainControl::SetValSubView);
    ClassDB::bind_method(D_METHOD("sub_view_getter"), &MainControl::GetValSubView);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Sub View"), "sub_view_setter", "sub_view_getter");
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
            godot::ClassDB::register_abstract_class<DragonControlTop>();
            godot::ClassDB::register_class<DragonControlKeyboard>();
            godot::ClassDB::register_class<DragonControlJoystick>();
            godot::ClassDB::register_class<DragonAnimator>();
            godot::ClassDB::register_class<CameraControl>();
            godot::ClassDB::register_class<GameTimer>();
            godot::ClassDB::register_class<CheatSheet>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}