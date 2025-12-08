#include "Control_Main.h"
#include "Control_Scene_Home.h"
#include "Control_Scene_TD.h"
#include "Control_Scene_Practice.h"
#include "Control_Scene_Tutorial.h"
#include "DragonAnimator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "SaveManager.h"
#include "DragonControlKeyboard.h"
#include "DragonControlJoystick.h"
#include "Control_Camera.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/resource_loader.hpp> // for ResourceLoader
#include <godot_cpp/classes/packed_scene.hpp> // for PackedScene
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>

using namespace godot;


Control_Main::Control_Main()
{
}


Control_Main::~Control_Main()
{
}


void Control_Main::_ready()
{
    // Initialize SaveManager for both editor and runtime
    if (!save_manager) {
        save_manager = memnew(SaveManager);
        add_child(save_manager);
        UtilityFunctions::print("SaveManager initialized in _ready()");
    }
    LoadSettings();

    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }

    Switch_Scene("Scene_Home");

    if (enable_headset) 
    {
        DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
        Ref<XRInterface> xr_interface = XRServer::get_singleton()->find_interface("OpenXR");
        if (!xr_interface.is_valid()) 
        {
            UtilityFunctions::printerr("[OpenXR] Interface not found. Ensure OpenXR is available in this export template.");
        } 
        else 
        {
            UtilityFunctions::print("[OpenXR] Interface found: ", xr_interface->get_name());
            if (xr_interface->initialize()) 
            {
                UtilityFunctions::print("[OpenXR] Initialize OK");
                // Set primary interface for XR rendering
                XRServer::get_singleton()->set_primary_interface(xr_interface);

                Viewport* main_viewport = get_viewport();
                if (main_viewport) 
                {
                    main_viewport->set_use_xr(true);
                }
            } else {
                UtilityFunctions::printerr("[OpenXR] Initialize FAILED. Make sure an OpenXR runtime (SteamVR/Meta/WMR) is installed and set as active, and start it before launching the game.");
            }
        }
        Engine* engine = Engine::get_singleton();
        if (engine) 
        {
            engine->set_physics_ticks_per_second(60);
        }
    }
    else 
    {
        Node3D* xr_node = get_parent()->get_node<Node3D>("Camera_Main/XR");
        xr_node->set_visible(false);
    }

    camera_main = get_parent()->get_node<Node3D>("Camera_Main");
}


void Control_Main::Switch_Scene(const String &scene_name)
{
    UtilityFunctions::print("Button clicked!");
    UtilityFunctions::print("Switching to scene: " + scene_name);
    Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://Scenes/" + scene_name + ".tscn");
    if (scene.is_valid()) 
    {
        // Clean up current scene before loading new one
        Node *parent = get_parent();
        PackedStringArray scene_names = PackedStringArray();
        scene_names.push_back("Scene_Home");
        scene_names.push_back("Scene_Tutorial");
        scene_names.push_back("Scene_Practice");
        scene_names.push_back("Scene_TD");

        
        for (int i = 0; i < scene_names.size(); i++) 
        {
            String current_scene_name = scene_names[i];
            if (current_scene_name != scene_name) 
            {
                Node *current_scene = parent->get_node_or_null(NodePath(current_scene_name));
                if (current_scene) 
                {
                    current_scene->queue_free();
                }
            }
        }
        
        Node *new_scene = scene->instantiate();
        get_parent()->call_deferred("add_child", new_scene);
        new_scene->set_name(scene_name);

        if (scene_name == "Scene_TD")
        {
            camera_main->reparent(new_scene->get_node<Node>("Dragon"));
            camera_main->set_position(Vector3(0, 0, 0));
            Node *node_cheat_sheet = memnew(CheatSheet);
            new_scene->get_node<Node>("Dragon")->add_child(node_cheat_sheet);
            node_cheat_sheet->set_name("CheatSheet");
            new_scene->add_child(memnew(Control_Scene_TD()));
        }
        // Scene_Tutorial已经在.tscn文件中包含了Control_Scene_Tutorial节点，不需要手动添加
        if (scene_name == "Scene_Home")
        {
            // Scene_Home需要手动添加Control_Scene_Home节点，因为.tscn文件中没有包含
            Control_Scene_Home* control_home = memnew(Control_Scene_Home());
            new_scene->add_child(control_home);
        }
    }
    else
    {
        UtilityFunctions::printerr("Failed to load", scene_name, ".tscn");
    }
}


/**
 * @brief the setter for enable_headset
 * @param val the value to set
 */
void Control_Main::SetValEnableHeadset(bool val)
{
    enable_headset = val;
    // UtilityFunctions::print("SetValEnableHeadset called with value: ", val);

    bool current_xr_enabled = ProjectSettings::get_singleton()->get_setting("xr/openxr/enabled");
    // UtilityFunctions::print("Current XR enabled: ", current_xr_enabled);
    if (current_xr_enabled != val) 
    {
        ProjectSettings::get_singleton()->set_setting("xr/openxr/enabled", val);
        Error err = ProjectSettings::get_singleton()->save(); // save the settings to project.godot file
        if (err != OK) 
        {
            UtilityFunctions::printerr("Failed to save project settings: ", err);
        }
    }

    if (!is_loading_settings && save_manager) 
    {
        SaveSettings();
    }

    notify_property_list_changed(); // Ensure Inspector refreshes to reflect the new value when inspecting the live node
}


/**
 * @brief the setter for sub_view
 * @param val the value to set
 */
void Control_Main::SetValSubView(bool val)
{
    sub_view = val;
    if (!is_loading_settings && save_manager) 
    {
        SaveSettings();
    }
    notify_property_list_changed();
}


/**
 * @brief the setter for debug
 * @param val the value to set
 */
void Control_Main::SetValDebug(bool val)
{
    debug = val;
    if (!is_loading_settings && save_manager) 
    {
        SaveSettings();
    }
    notify_property_list_changed();
}


/**
 * @brief the setter for language
 * @param val the value to set
 */
void Control_Main::SetValLanguage(int val)
{
    language = static_cast<Language>(val);
    if (!is_loading_settings && save_manager) 
    {
        SaveSettings();
    }
    notify_property_list_changed();
}

/**
 * @brief set the badge value (0-3)
 * @param val the badge value to set
 */
void Control_Main::SetValBadge(int val)
{
    badge = val;
    if (!is_loading_settings && save_manager) 
    {
        SaveSettings();
    }
    notify_property_list_changed();
}


void Control_Main::_bind_methods()
{
    // Expose Switch_Scene so it can be called via Node.call
    ClassDB::bind_method(D_METHOD("Switch_Scene", "scene_name"), &Control_Main::Switch_Scene);
    
    ClassDB::bind_method(D_METHOD("language_setter", "value"), &Control_Main::SetValLanguage);
    ClassDB::bind_method(D_METHOD("language_getter"), &Control_Main::GetValLanguage);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "Language", PROPERTY_HINT_ENUM, "English,Chinese"), "language_setter", "language_getter");
    ClassDB::bind_method(D_METHOD("enable_headset_setter", "value"), &Control_Main::SetValEnableHeadset);
    ClassDB::bind_method(D_METHOD("enable_headset_getter"), &Control_Main::GetValEnableHeadset);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Enable Headset"), "enable_headset_setter", "enable_headset_getter");
    ClassDB::bind_method(D_METHOD("sub_view_setter", "value"), &Control_Main::SetValSubView);
    ClassDB::bind_method(D_METHOD("sub_view_getter"), &Control_Main::GetValSubView);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Sub View"), "sub_view_setter", "sub_view_getter");
    ClassDB::bind_method(D_METHOD("debug_setter", "value"), &Control_Main::SetValDebug);
    ClassDB::bind_method(D_METHOD("debug_getter"), &Control_Main::GetValDebug);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Debug"), "debug_setter", "debug_getter");
    ClassDB::bind_method(D_METHOD("badge_setter", "value"), &Control_Main::SetValBadge);
    ClassDB::bind_method(D_METHOD("badge_getter"), &Control_Main::GetValBadge);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "Badge", PROPERTY_HINT_RANGE, "0,3"), "badge_setter", "badge_getter");
}


void Control_Main::LoadSettings()
{
    if (!save_manager) {
        UtilityFunctions::printerr("SaveManager not initialized in LoadSettings()");
        return;
    }
    
    is_loading_settings = true; // Set flag to prevent saving during loading
    
    Dictionary settings = save_manager->Settings_Load();
    
    if (settings.has("language")) {
        SetValLanguage(static_cast<int>(settings["language"]));
    }
    
    if (settings.has("enable_headset")) {
        SetValEnableHeadset(static_cast<bool>(settings["enable_headset"]));
    }
    
    if (settings.has("sub_view")) {
        SetValSubView(static_cast<bool>(settings["sub_view"]));
    }
    
    if (settings.has("debug")) {
        SetValDebug(static_cast<bool>(settings["debug"]));
    }
    
    if (settings.has("badge")) {
        SetValBadge(static_cast<int>(settings["badge"]));
    }
    
    is_loading_settings = false; // Clear flag after loading
    UtilityFunctions::print("Settings loaded and applied");
    notify_property_list_changed();
}


void Control_Main::SaveSettings()
{
    if (!save_manager) 
    {
        UtilityFunctions::printerr("SaveManager not initialized in SaveSettings()");
        return;
    }
    
    Dictionary settings;
    settings["language"] = GetValLanguage();
    settings["enable_headset"] = GetValEnableHeadset();
    settings["sub_view"] = GetValSubView();
    settings["debug"] = GetValDebug();
    settings["badge"] = GetValBadge();
    
    save_manager->Settings_Save(settings);
    UtilityFunctions::print("Settings saved");
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
            godot::ClassDB::register_class<Control_Main>();
            godot::ClassDB::register_class<Control_Scene_TD>();
            godot::ClassDB::register_class<Control_Scene_Practice>();
            godot::ClassDB::register_class<Control_Scene_Tutorial>();
            godot::ClassDB::register_class<Control_Scene_Home>();
            godot::ClassDB::register_abstract_class<DragonControlTop>();
            godot::ClassDB::register_class<DragonControlKeyboard>();
            godot::ClassDB::register_class<DragonControlJoystick>();
            godot::ClassDB::register_class<DragonAnimator>();
            godot::ClassDB::register_class<Control_Camera>();
            godot::ClassDB::register_class<GameTimer>();
            godot::ClassDB::register_class<CheatSheet>();
            godot::ClassDB::register_class<SaveManager>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}