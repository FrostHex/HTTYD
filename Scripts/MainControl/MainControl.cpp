#include "MainControl.h"
#include "TD_MainControl.h"
#include "DragonAnimator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "SaveManager.h"
#include "DragonControlKeyboard.h"
#include "DragonControlJoystick.h"
#include "CameraControl.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/resource_loader.hpp> // for ResourceLoader
#include <godot_cpp/classes/packed_scene.hpp> // for PackedScene

using namespace godot;


MainControl::MainControl()
{
}


MainControl::~MainControl()
{
}


void MainControl::_ready()
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }

    camera_main = get_parent()->get_node<Node3D>("Camera_Main");
    const char* button_names[] = { "Button_TD", "Button_Tutorial" };
    for (const char* btn_name : button_names) 
    {
        Node *button = get_parent()->get_node<Node>(String("Scene_Home/SubViewportContainer/SubViewport/CanvasLayer/Control/") + btn_name);
        if (button) 
        {
            String scene_name = "Scene_" + String(btn_name).replace("Button_", "");
            button->connect("pressed", Callable(this, "_on_button_pressed").bind(scene_name));
        }
    }
}


void MainControl::Switch_Scene(const String &scene_name)
{
    UtilityFunctions::print("Button clicked!");
    UtilityFunctions::print("Switching to scene: " + scene_name);
    Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://Scenes/" + scene_name + ".tscn");
    if (scene.is_valid()) 
    {
        if (scene_name != "Scene_Home")
        {
            Node *current = get_parent()->get_node<Node>("Scene_Home");
            current->queue_free();
        }
        Node *new_scene = scene->instantiate();
        get_parent()->add_child(new_scene);

        if (scene_name == "Scene_TD")
        {
            camera_main->reparent(new_scene->get_node<Node>("Dragon"));
            camera_main->set_position(Vector3(0, 0, 0));
            Node *node_cheat_sheet = memnew(CheatSheet);
            new_scene->get_node<Node>("Dragon")->add_child(node_cheat_sheet);
            node_cheat_sheet->set_name("CheatSheet");
            new_scene->add_child(memnew(TD_MainControl));
        }
    }
    else
    {
        UtilityFunctions::printerr("Failed to load", scene_name, ".tscn");
    }
}


void MainControl::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_on_button_pressed", "button"), &MainControl::Switch_Scene);
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
            godot::ClassDB::register_class<TD_MainControl>();
            godot::ClassDB::register_abstract_class<DragonControlTop>();
            godot::ClassDB::register_class<DragonControlKeyboard>();
            godot::ClassDB::register_class<DragonControlJoystick>();
            godot::ClassDB::register_class<DragonAnimator>();
            godot::ClassDB::register_class<CameraControl>();
            godot::ClassDB::register_class<GameTimer>();
            godot::ClassDB::register_class<CheatSheet>();
            godot::ClassDB::register_class<SaveManager>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}