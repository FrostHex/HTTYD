#include "Control_Main.h"
#include "Control_Scene_Home.h"
#include "Control_Scene_TD.h"
#include "Control_Scene_Practice.h"
#include "Control_Scene_Tutorial.h"
#include "Control_Scene_Dodge.h"
#include "Dragon_Animator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "SaveManager.h"
#include "SunsetBridge.h"
#include "Dragon_Pilot_Keyboard.h"
#include "Dragon_Pilot_Joystick.h"
#include "Dragon_Pilot_Dodge.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/classes/resource_loader.hpp> // for ResourceLoader
#include <godot_cpp/classes/packed_scene.hpp> // for PackedScene
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>

using namespace godot;


Control_Main::Control_Main()
{
}


Control_Main::~Control_Main()
{
}


void Control_Main::_ready()
{
    // ProjectSettings::get_singleton()->set_setting("xr/openxr/enabled", false);

    save_manager = get_node<SaveManager>("SaveManager"); // initialize SaveManager for both editor and runtime
    LoadSettings();

    if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

    Switch_Scene("Scene_Home");
    ctrl_camera = get_node<Control_Camera>("Control_Camera");
    camera_main = get_parent()->get_node<Node3D>("Camera_Main");

    if (enable_headset) 
    {
        UtilityFunctions::print("Starting XR interface initialization...");
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

                ctrl_camera->ResetVRTransform();
            }
            else
            {
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
}


void Control_Main::Switch_Scene(const String &scene_name)
{
    // UtilityFunctions::print("Button clicked!");
    // UtilityFunctions::print("Switching to scene: " + scene_name);
    Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://Scenes/" + scene_name + ".tscn");
    if (scene.is_valid()) 
    {
        // clean up current scene before loading new one
        Node *parent = get_parent();
        PackedStringArray scene_names = PackedStringArray();
        scene_names.push_back("Scene_Home");
        scene_names.push_back("Scene_Tutorial");
        scene_names.push_back("Scene_Practice");
        scene_names.push_back("Scene_TD");
        scene_names.push_back("Scene_TD_VR");
        scene_names.push_back("Scene_Dodge");
        
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
            if (!enable_headset)
            {
                call_deferred("AttachSunshineClouds", scene_name, true);
            }
            else if (camera_main)
            {
                Node *xr_origin = camera_main->get_node_or_null(NodePath("XR/XROrigin"));
                if (xr_origin)
                {
                    Node *left_pointer = xr_origin->get_node_or_null(NodePath("LeftHand/FunctionPointer"));
                    Node *right_pointer = xr_origin->get_node_or_null(NodePath("RightHand/FunctionPointer"));
                    if (left_pointer)
                    {
                        left_pointer->set("enabled", false);
                        left_pointer->set("show_laser", 0);
                    }
                    if (right_pointer)
                    {
                        right_pointer->set("enabled", false);
                        right_pointer->set("show_laser", 0);
                    }
                }
            }
            Node *node_cheat_sheet = memnew(CheatSheet);
            new_scene->get_node<Node>("Dragon")->add_child(node_cheat_sheet);
            node_cheat_sheet->set_name("CheatSheet");
        }

        // if (scene_name == "Scene_Dodge")
        // {
        // }
            
        // if (scene_name == "Scene_Home")
        // {
        // }

        if (scene_name != "Scene_Home")
        {  
            call_deferred("AttachCamera", scene_name); // resolve target species at deferred execution time to avoid binding to stale Toothless node
        }
    }
    else
    {
        UtilityFunctions::printerr("Failed to load", scene_name, ".tscn");
    }
}

void Control_Main::AttachCamera(const String &scene_name)
{
    Node *target_scene = get_parent()->get_node_or_null(NodePath(scene_name));
    Node *species_slot = target_scene->get_node_or_null(NodePath("Dragon/SpeciesSlot"));
    if (!species_slot || species_slot->get_child_count() <= 0)
    {
        return;
    }
    Node *latest_species = species_slot->get_child(species_slot->get_child_count() - 1);
    if (!latest_species)
    {
        return;
    }
    Node *socket_back = latest_species->get_node_or_null(NodePath("Sockets/Socket_Back_Mount/Socket_Back"));
    if (!camera_main)
    {
        return;
    }
    camera_main->reparent(socket_back);
    camera_main->set_position(Vector3(0, 0, 0));
    camera_main->set_rotation(Vector3(0, 0, 0));
}

void Control_Main::AttachSunshineClouds(const String &scene_name, bool attach)
{
    Node *target_scene = get_parent()->get_node_or_null(NodePath(scene_name));
    if (!target_scene)
    {
        // Scene add_child is also deferred in Switch_Scene, so retry next idle frame.
        if (attach)
        {
            call_deferred("AttachSunshineClouds", scene_name, attach);
        }
        return;
    }
    if (!target_scene->is_inside_tree())
    {
        if (attach)
        {
            call_deferred("AttachSunshineClouds", scene_name, attach);
        }
        return;
    }

    Node *sky_node = target_scene->get_node_or_null(NodePath("Sky3D"));
    if (sky_node)
    {
        sky_node->set("wind_speed", attach ? 0.0f : 1.0f);
    }

    Node *clouds_node = target_scene->get_node_or_null(NodePath("SunshineClouds"));
    if (!attach)
    {
        if (!clouds_node)
        {
            return;
        }

        Variant res_var = clouds_node->get("clouds_resource");
        Object *res_obj = res_var;
        if (res_obj)
        {
            res_obj->set("clouds_coverage", 0.0f);
        }
        clouds_node->queue_free();
        return;
    }

    if (clouds_node)
    {
        return;
    }

    Ref<PackedScene> clouds_scene = ResourceLoader::get_singleton()->load("res://Scenes/SunshineClouds.tscn");
    if (!clouds_scene.is_valid())
    {
        UtilityFunctions::printerr("SunshineClouds.tscn not found.");
        return;
    }

    clouds_node = clouds_scene->instantiate();
    if (!clouds_node)
    {
        return;
    }

    target_scene->add_child(clouds_node);
    clouds_node->set_name("SunshineClouds");
    clouds_node->call_deferred("clouds_res_added");
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

    // bool current_xr_enabled = false;
    // Ref<FileAccess> project_file = FileAccess::open("res://project.godot", FileAccess::READ);
    // if (project_file.is_valid()) 
    // {
    //     String file_content = project_file->get_as_text();
    //     current_xr_enabled = file_content.find("openxr/enabled=true") != -1;
    // }

    // UtilityFunctions::print("Current XR enabled: ", current_xr_enabled);
    if (current_xr_enabled != val) 
    {
        UtilityFunctions::print("Current XR setting (", current_xr_enabled, ") differs from new value (", val, "). Updating Project Settings.");
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
    ClassDB::bind_method(D_METHOD("AttachCamera", "scene_name"), &Control_Main::AttachCamera);
    ClassDB::bind_method(D_METHOD("AttachSunshineClouds", "scene_name", "attach"), &Control_Main::AttachSunshineClouds);
    
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
        SetValEnableHeadset(settings["enable_headset"].operator bool());
    }
    
    if (settings.has("sub_view")) {
        SetValSubView(settings["sub_view"].operator bool());
    }
    
    if (settings.has("debug")) {
        SetValDebug(settings["debug"].operator bool());
    }
    
    if (settings.has("badge")) {
        SetValBadge(static_cast<int>(settings["badge"]));
    }
    
    is_loading_settings = false; // Clear flag after loading
    // UtilityFunctions::print("Settings loaded and applied");
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
            godot::ClassDB::register_class<Control_Scene_Dodge>();
            godot::ClassDB::register_abstract_class<Dragon_Pilot_Top>();
            godot::ClassDB::register_class<Dragon_Pilot_Keyboard>();
            godot::ClassDB::register_class<Dragon_Pilot_Joystick>();
            godot::ClassDB::register_class<Dragon_Pilot_Dodge>();
            godot::ClassDB::register_class<Dragon_Animator>();
            godot::ClassDB::register_class<Control_Camera>();
            godot::ClassDB::register_class<GameTimer>();
            godot::ClassDB::register_class<CheatSheet>();
            godot::ClassDB::register_class<SaveManager>();
            godot::ClassDB::register_class<SunsetBridge>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}