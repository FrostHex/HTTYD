// ==================== Control_Main.cpp ====================
#include "Control_Main.h"
#include "Control_Scene_Home.h"
#include "Control_Scene_TD.h"
#include "Control_Scene_Practice.h"
#include "Control_Scene_Tutorial.h"
#include "Control_Scene_Dodge.h"
#include "Dragon_Animator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "Settings.h"
#include "SunsetBridge.h"
#include "Dragon_Pilot_Keyboard.h"
#include "Dragon_Pilot_Joystick.h"
#include "Dragon_Pilot_Dodge.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/xr_interface.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>

using namespace godot;

Control_Main::Control_Main() {}
Control_Main::~Control_Main() {}

void Control_Main::_ready()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    Switch_Scene("Scene_Home");
    ctrl_camera = get_node<Control_Camera>("Control_Camera");
    camera_main = get_parent()->get_node<Node3D>("Camera_Main");

    if (Settings::GetSingleton()->GetValEnableHeadset()) 
    {
        UtilityFunctions::print("Starting XR interface initialization...");
        DisplayServer::get_singleton()->window_set_vsync_mode(DisplayServer::VSYNC_DISABLED);
        Ref<XRInterface> xr_interface = XRServer::get_singleton()->find_interface("OpenXR");
        if (!xr_interface.is_valid())
        {
            UtilityFunctions::printerr("[OpenXR] Interface not found.");
        } 
        else 
        {
            UtilityFunctions::print("[OpenXR] Interface found: ", xr_interface->get_name());
            if (xr_interface->initialize()) 
            {
                UtilityFunctions::print("[OpenXR] Initialize OK");
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
                UtilityFunctions::printerr("[OpenXR] Initialize FAILED.");
            }
        }
        Engine::get_singleton()->set_physics_ticks_per_second(60);
    }
    else 
    {
        Node3D* xr_node = get_parent()->get_node<Node3D>("Camera_Main/XR");
        if (xr_node) xr_node->set_visible(false);
    }
}

void Control_Main::Switch_Scene(const String &scene_name)
{
    Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://Scenes/" + scene_name + ".tscn");
    if (scene.is_valid()) 
    {
        Node *parent = get_parent();
        PackedStringArray scene_names;
        scene_names.push_back("Scene_Home");
        scene_names.push_back("Scene_Tutorial");
        scene_names.push_back("Scene_Practice");
        scene_names.push_back("Scene_TD");
        scene_names.push_back("Scene_TD_VR");
        scene_names.push_back("Scene_Dodge");
        
        for (int i = 0; i < scene_names.size(); i++) 
        {
            String current = scene_names[i];
            if (current != scene_name) 
            {
                Node *node = parent->get_node_or_null(NodePath(current));
                if (node) node->queue_free();
            }
        }
        
        Node *new_scene = scene->instantiate();
        get_parent()->call_deferred("add_child", new_scene);
        new_scene->set_name(scene_name);

        if (scene_name == "Scene_TD")
        {
            if (Settings::GetSingleton()->GetValEnableHeadset())
            {
                call_deferred("AttachSunshineClouds", scene_name, true);
            }
            else if (camera_main)
            {
                Node *xr_origin = camera_main->get_node_or_null("XR/XROrigin");
                if (xr_origin)
                {
                    Node *left = xr_origin->get_node_or_null("LeftHand/FunctionPointer");
                    Node *right = xr_origin->get_node_or_null("RightHand/FunctionPointer");
                    if (left) { left->set("enabled", false); left->set("show_laser", 0); }
                    if (right) { right->set("enabled", false); right->set("show_laser", 0); }
                }
            }
            Node *node_cheat_sheet = memnew(CheatSheet);
            new_scene->get_node<Node>("Dragon")->add_child(node_cheat_sheet);
            node_cheat_sheet->set_name("CheatSheet");
        }

        if (scene_name != "Scene_Home")
        {  
            call_deferred("AttachCamera", scene_name);
        }
    }
    else
    {
        UtilityFunctions::printerr("Failed to load ", scene_name, ".tscn");
    }
}

void Control_Main::AttachCamera(const String &scene_name)
{
    Node *target_scene = get_parent()->get_node_or_null(NodePath(scene_name));
    if (!target_scene) return;
    
    Node *species_slot = target_scene->get_node_or_null("Dragon/SpeciesSlot");
    if (!species_slot || species_slot->get_child_count() <= 0) return;
    
    Node *latest_species = species_slot->get_child(species_slot->get_child_count() - 1);
    if (!latest_species) return;

    Node *socket_back = latest_species->get_node_or_null("Sockets/Socket_Back_Mount/Socket_Back");
    if (!camera_main || !socket_back) return;

    camera_main->reparent(socket_back);
    camera_main->set_position(Vector3(0, 0, 0));
    camera_main->set_rotation(Vector3(0, 0, 0));
}

void Control_Main::AttachSunshineClouds(const String &scene_name, bool attach)
{
    Node *target_scene = get_parent()->get_node_or_null(NodePath(scene_name));
    if (!target_scene)
    {
        if (attach) call_deferred("AttachSunshineClouds", scene_name, attach);
        return;
    }
    if (!target_scene->is_inside_tree())
    {
        if (attach) call_deferred("AttachSunshineClouds", scene_name, attach);
        return;
    }

    Node *sky_node = target_scene->get_node_or_null("Sky3D");
    if (sky_node) sky_node->set("wind_speed", attach ? 0.0f : 1.0f);

    Node *clouds_node = target_scene->get_node_or_null("SunshineClouds");
    if (!attach)
    {
        if (clouds_node)
        {
            Variant res_var = clouds_node->get("clouds_resource");
            if (Object* res = res_var) res->set("clouds_coverage", 0.0f);
            clouds_node->queue_free();
        }
        return;
    }

    if (clouds_node) return;

    Ref<PackedScene> clouds_scene = ResourceLoader::get_singleton()->load("res://Scenes/SunshineClouds.tscn");
    if (!clouds_scene.is_valid()) return;

    clouds_node = clouds_scene->instantiate();
    if (!clouds_node) return;

    target_scene->add_child(clouds_node);
    clouds_node->set_name("SunshineClouds");
    clouds_node->call_deferred("clouds_res_added");
}

void Control_Main::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Switch_Scene", "scene_name"), &Control_Main::Switch_Scene);
    ClassDB::bind_method(D_METHOD("AttachCamera", "scene_name"), &Control_Main::AttachCamera);
    ClassDB::bind_method(D_METHOD("AttachSunshineClouds", "scene_name", "attach"), &Control_Main::AttachSunshineClouds);
}

extern "C" GDE_EXPORT GDExtensionBool gdextension_init(GDExtensionInterfaceGetProcAddress get_proc_addr, GDExtensionClassLibraryPtr lib, GDExtensionInitialization *init) 
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
            godot::ClassDB::register_class<Settings>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}