// ======================================================
// control_main.cpp - comprehensive documentation
// how to train your dragon vr project
// ======================================================

// note: this file provides detailed inline documentation explaining the complex
// c++ macro system used for settings management in godot engine

// standard header files - scene controllers
#include "Control_Main.h"
#include "Control_Scene_Home.h"
#include "Control_Scene_TD.h"
#include "Control_Scene_Practice.h"
#include "Control_Scene_Tutorial.h"
#include "Control_Scene_Dodge.h"

// utility and dragon control modules
#include "Dragon_Animator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "SaveManager.h"
#include "SunsetBridge.h"
#include "Dragon_Pilot_Keyboard.h"
#include "Dragon_Pilot_Joystick.h"
#include "Dragon_Pilot_Dodge.h"

// godot engine headers for c++ extension
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


// ======================================================
// setting specification system
// ======================================================

namespace
{
    // internal structure for describing a single setting
    // this bridges between runtime property access and static type info
    struct setting_spec
    {
        const char *key;                                  // save file keystring identifier
        std::function<variant(const control_main *)> get; // thread-safe getter
        std::function<void(control_main *, const variant &)> set; // non-const setter
    };

    // auto-generate setting specifications from the master list
    // this function returns static data that doesn't require lifetime management
    const setting_spec *get_setting_specs(int &count)
    {
        // static array size is determined at compile-time but accessible at runtime
        static const setting_spec specs[] =
        {
            // expansion flow:
            // 1. make_setting_spec becomes template in context
            // 2. each row in control_main_setting_list expands to lambda + method
            #define make_setting_spec(ui_exposed, propname, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
            { \
                key, \                                                     // save key like "language"
                [](const control_main *self) { return self->getval##propname(); }, \  // auto-getter ties to cc method
                [](control_main *self, const variant &value) { self->setval##propname(static_cast<type>(value)); } \ // auto-setter with type cast
            },

            // generate all setting specifications from the master list
            control_main_setting_list(make_setting_spec)

            // cleanup to prevent macro leakage
            #undef make_setting_spec
        };

        // compute count for caller - eliminate manual counting
        count = sizeof(specs) / sizeof(specs[0]);
        return specs;
    }
}

// ======================================================
// construction and destruction
// ======================================================

control_main::control_main()
{
    // default constructor - init may be misleading as members were initialized via
    // member initializer in header file using the control_main_setting_list macro
}

control_main::~control_main()
{
    // destructor deliberately empty as godot handles memory management
    // node3d and other smart pointers auto-released
}

// ======================================================
// core initialization sequence
// ======================================================

void control_main::_ready()
{
    // _ready is godot's "on ready" callback - runs after scene tree construction
    // safe to query scene tree and perform setup that requires scene context

    save_manager = get_node<savemanager>("savemanager");

    // load persisted settings from json - will write defaults if file missing
    loadsettings();

    // skip all scene setup when running inside godot editor
    // prevents testing errors during scene editing
    if (engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    // initialize to home scene as default application state
    switch_scene("scene_home");

    // cache key nodes the logic touches frequently
    ctrl_camera = get_node<control_camera>("control_camera");
    camera_main = get_parent()->get_node<node3d>("camera_main");

    // ==================================================
    // virtual reality initialization sequence
    // ==================================================
    if (enable_headset)
    {
        utilityfunctions::print("starting xr interface initialization...");

        // development note: v-sync must be disabled for vr mode
        // prevents double buffering pipeline conflicts
        display_server::get_singleton()->window_set_vsync_mode(display_server::vsync_disabled);

        // query openxr provider for hmd support
        ref<xrinteface> xr_interface = xrserver::get_singleton()->find_interface("openxr");
        if (!xr_interface.is_valid())
        {
            utilityfunctions::printerr("[openxr] interface not found.");
        }
        else
        {
            utilityfunctions::print("[openxr] interface found: ", xr_interface->get_name());

            if (xr_interface->initialize())
            {
                utilityfunctions::print("[openxr] initialize ok");
                xrserver::get_singleton()->set_primary_interface(xr_interface);

                // enable xr rendering pipeline on main viewport
                viewport* main_viewport = get_viewport();
                if (main_viewport)
                {
                    main_viewport->set_use_xr(true);
                }

                // reset camera to vr origin - removes any editor transforms
                ctrl_camera->resetvrtransform();
            }
            else
            {
                utilityfunctions::printerr("[openxr] initialize failed.");
            }
        }

        // configure physics to run at stable 60 hz for vr comfort
        engine::get_singleton()->set_physics_ticks_per_second(60);
    }
    else
    {
        // in non-vr modes, hide xr nodes to prevent visual clutter
        node3d* xr_node = get_parent()->get_node<node3d>("camera_main/xr");
        if (xr_node) xr_node->set_visible(false);
    }
}

// ======================================================
// scene management system
// ======================================================

void control_main::switch_scene(const string &scene_name)
{
    // load requested scene file from resources
    ref<packedscene> scene = resourceloader::get_singleton()->load("res://scenes/" + scene_name + ".tscn");
    if (scene.is_valid())
    {
        // get parent container for current state rotation
        node *parent = get_parent();

        // centralized scene whitelist - controls what scenes we can rotate to
        packedstringarray scene_names;
        scene_names.push_back("scene_home");       // main lobby
        scene_names.push_back("scene_tutorial");    // basic flight tutorial
        scene_names.push_back("scene_practice");    // sandbox practice mode
        scene_names.push_back("scene_td");          // target drone challenge
        scene_names.push_back("scene_td_vr");       // vr version of target drones
        scene_names.push_back("scene_dodge");       // obstacle dodge mode

        // cleanup previous scenes - only the requested scene survives
        // note: queue_free() schedules cleanup for end of frame to prevent crashes
        for (int i = 0; i < scene_names.size(); i++)
        {
            string current = scene_names[i];
            if (current != scene_name)
            {
                node *node = parent->get_node_or_null(nodepath(current));
                if (node) node->queue_free();
            }
        }

        // instantiate the new scene and add to tree
        node *new_scene = scene->instantiate();
        get_parent()->call_deferred("add_child", new_scene);
        new_scene->set_name(scene_name);

        // ==================================================
        // post-scene-creation configuration
        // ==================================================

        if (scene_name == "scene_td")
        {
            // target drone mode specific configuration

            if (!enable_headset)
            {
                // non-vr mode: attach external sky/cloud system
                call_deferred("attachsunshineclouds", scene_name, true);
            }
            else if (camera_main)
            {
                // vr mode: disable instructor hand pointers (performance)
                node *xr_origin = camera_main->get_node_or_null("xr/xrorigin");
                if (xr_origin)
                {
                    node *left = xr_origin->get_node_or_null("lefthand/functionpointer");
                    node *right = xr_origin->get_node_or_null("righthand/functionpointer");
                    if (left) { left->set("enabled", false); left->set("show_laser", 0); }
                    if (right) { right->set("enabled", false); right->set("show_laser", 0); }
                }
            }

            // attach embedded cheat sheet ui mode selector
            node *node_cheat_sheet = memnew(cheatsheet);
            new_scene->get_node<node>("dragon")->add_child(node_cheat_sheet);
            node_cheat_sheet->set_name("cheatsheet");
        }

        if (scene_name != "scene_home")
        {
            // attach camera mount rig - applies to all modes except home
            call_deferred("attachcamera", scene_name);
        }
    }
    else
    {
        utilityfunctions::printerr("failed to load ", scene_name, ".tscn");
    }
}

// ======================================================
// camera management for dynamic mounting
// ======================================================

void control_main::attachcamera(const string &scene_name)
{
    // locate target scene in parent tree
    node *target_scene = get_parent()->get_node_or_null(nodepath(scene_name));
    if (!target_scene) return;

    // finds the current dragon species being rendered
    node *species_slot = target_scene->get_node_or_null("dragon/speciesslot");
    if (!species_slot || species_slot->get_child_count() <= 0) return;

    // gets the actual dragon instance (usually the last added)
    node *latest_species = species_slot->get_child(species_slot->get_child_count() - 1);
    if (!latest_species) return;

    // locates attach point socket on dragon back
    node *socket_back = latest_species->get_node_or_null("sockets/socket_back_mount/socket_back");
    if (!camera_main || !socket_back) return;

    // reparent camera to dragon spine
    camera_main->reparent(socket_back);
    camera_main->set_position(vector3(0, 0, 0));
    camera_main->set_rotation(vector3(0, 0, 0));
}

// ======================================================
// environment asset management
// ======================================================

void control_main::attachsunshineclouds(const string &scene_name, bool attach)
{
    // manages dynamic loading/removal of the sunshine clouds sky system
    // used to configure non-vr modes where environmental lighting is controlled by code

    node *target_scene = get_parent()->get_node_or_null(nodepath(scene_name));
    if (!target_scene)
    {
        // if scene not ready yet, re-schedule for later frame
        if (attach) call_deferred("attachsunshineclouds", scene_name, attach);
        return;
    }

    if (!target_scene->is_inside_tree())
    {
        // similarly wait if scene not fully initialized
        if (attach) call_deferred("attachsunshineclouds", scene_name, attach);
        return;
    }

    // adjust sky wind speed based on desired cloud behavior
    node *sky_node = target_scene->get_node_or_null("sky3d");
    if (sky_node) sky_node->set("wind_speed", attach ? 0.0f : 1.0f);

    // handle clouds attachment system
    node *clouds_node = target_scene->get_node_or_null("sunshineclouds");
    if (!attach)
    {
        // detach and cleanup when leaving scene modes
        if (clouds_node)
        {
            variant res_var = clouds_node->get("clouds_resource");
            if (object* res = res_var) res->set("clouds_coverage", 0.0f);
            clouds_node->queue_free();
        }
        return;
    }

    if (clouds_node) return;  // already attached

    // load sunshineclouds scene from resource library
    ref<packedscene> clouds_scene = resourceloader::get_singleton()->load("res://scenes/sunshineclouds.tscn");
    if (!clouds_scene.is_valid()) return;

    // instantiate and add to target scene
    clouds_node = clouds_scene->instantiate();
    if (!clouds_node) return;

    target_scene->add_child(clouds_node);
    clouds_node->set_name("sunshineclouds");
    clouds_node->call_deferred("clouds_res_added");  // frame-delayed initialization
}

// ======================================================
// property setter implementations
// ======================================================

// custom setter for enable_headset - requires projectsettings sync
void control_main::setvalenableheadset(bool val)
{
    enable_headset = val;

    // validate that project settings are synchronized globally
    bool current = projectsettings::get_singleton()->get_setting("xr/openxr/enabled");
    if (current != val)
    {
        projectsettings::get_singleton()->set_setting("xr/openxr/enabled", val);
        projectsettings::get_singleton()->save();  // persist vr setting globally
    }

    // don't save while still loading settings to avoid write cycles
    if (!is_loading_settings && save_manager) savesettings();

    // notify godot property system of value change
    notify_property_list_changed();
}

// macro that generates generic property setters for all settings
// called from control_main_setting_list in header file
#define define_generic_setter(propname, member, type, ...) \
void control_main::setval##propname(type val) \
{ \
    member = val; \      // store the value in member variable
    if (!is_loading_settings && save_manager) savesettings(); \  // avoid recursive saves during load
    notify_property_list_changed(); \  // refresh property inspector
}

// automatically generate setter methods for remaining settings
// expansion happens at compile-time - no runtime overhead
define_generic_setter(language, language, int)      // enum: en/zh
define_generic_setter(sub_view, sub_view, bool)    // split-screen toggle
define_generic_setter(debug, debug, bool)          // debug overlay toggle
define_generic_setter(badge, badge, int)           // achievement tracker [0-3]

// cleanup macro scope
#undef define_generic_setter

// ======================================================
// godot integration - method registration
// ======================================================

void control_main::_bind_methods()
{
    // register public methods visible to godot and gdscript
    // equivalent to [export] in gdscript

    classdb::bind_method(d_method("switch_scene", "scene_name"), &control_main::switch_scene);
    classdb::bind_method(d_method("attachcamera", "scene_name"), &control_main::attachcamera);
    classdb::bind_method(d_method("attachsunshineclouds", "scene_name", "attach"), &control_main::attachsunshineclouds);
    classdb::bind_method(d_method("getexposedsettings"), &control_main::getexposedsettings);

    // macro to auto-bind property accessors
    // creates setter/getter pairs for godot property editor
    #define bind_setting(ui_exposed, propname, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
    classdb::bind_method(d_method(#propname "_setter", "value"), &control_main::setval##propname); \     // connects to setter
    classdb::bind_method(d_method(#propname "_getter"), &control_main::getval##propname); \                 // connects to getter
    add_property(propertyinfo(static_cast<variant::type>(variant_type), property_name, hint, hint_string), \  // defines editor ui element
    #propname "_setter", #propname "_getter");                                       // links to methods

    // generate all property bindings automatically
    control_main_setting_list(bind_setting)
    #undef bind_setting
}

// ======================================================
// setting exposure system for ui generation
// ======================================================

// return filtered list of settings that should appear in user interface
// called by gdscript when building settings panel
array control_main::getexposedsettings() const
{
    array arr;

    // filter settings ui using the ui_exposed flag from the macro list
    #define add_if_exposed(ui_exposed, propname, member, type, default_value, key, variant_type, hint, hint_string, property_name, label_key, label_fallback, label_path, label_suffix, setter_kind) \
    if (ui_exposed) \
    { \
        dictionary dict; \
        dict["prop_name"] = string(#propname);            // base property name
        dict["variant_type"] = static_cast<int>(variant_type); // godot data type
        dict["hint"] = static_cast<int>(hint);            // editor control type
        dict["hint_string"] = string(hint_string);          // values for enum, ranges for numeric
        dict["label_key"] = string(label_key);          // localization string id
        dict["label_fallback"] = string(label_fallback);   // raw text if no translation
        dict["label_suffix"] = string(label_suffix);        // visual append (colon, space, etc)
        dict["is_custom"] = (setter_kind == control_main_setter_custom);  // special handling flag
        arr.push_back(dict); \
    }

    control_main_setting_list(add_if_exposed)
    #undef add_if_exposed

    return arr;
}

// ======================================================
// settings persistence layer
// ======================================================

void control_main::loadsettings()
{
    // early-out if save system not initialized
    if (!save_manager)
    {
        utilityfunctions::printerr("savemanager not initialized in loadsettings()");
        return;
    }

    // prevent save calls during loading
    is_loading_settings = true;

    // load saved configuration from json file
    dictionary settings = save_manager->settings_load();
    bool wrote_defaults = false;

    // iterate over all settings definitions and apply saved values
    int spec_count = 0;
    const setting_spec *specs = get_setting_specs(spec_count);

    for (int i = 0; i < spec_count; i++)
    {
        stringname key(specs[i].key);

        // use saved value, fallback to default value from header initializer
        variant value = settings.has(key) ? settings[key] : specs[i].get(this);
        if (!settings.has(key))
        {
            // populate missing keys with system defaults if file didn't exist
            settings[key] = value;
            wrote_defaults = true;
        }

        specs[i].set(this, value);  // update internal state
    }

    is_loading_settings = false;
    notify_property_list_changed();  // refresh any inspector if open

    if (wrote_defaults)
    {
        // save initial defaults back to prevent future cycles
        save_manager->settings_save(settings);
    }
}

void control_main::savesettings()
{
    // guard against uninitialized save system
    if (!save_manager)
    {
        utilityfunctions::printerr("savemanager not initialized in savesettings()");
        return;
    }

    // collect all current values into dictionary
    dictionary settings = save_manager->settings_load();
    int spec_count = 0;
    const setting_spec *specs = get_setting_specs(spec_count);

    for (int i = 0; i < spec_count; i++)
    {
        settings[string(specs[i].key)] = specs[i].get(this);  // serialize current values
    }

    save_manager->settings_save(settings);  // persist to disk
}

// ======================================================
// godot extension initialization
// ======================================================

extern "c" gde_export gdextensionbool gdextension_init(
    gdextensioninterfacegetprocaddress get_proc_addr,
    gdextensionclasslibraryptr lib,
    gdextensioninitialization *init)
{
    // background: this is standard godot cpp extension entry point
    // automatically called by the engine when plugin loads

    godot::gdextensionbinding::initobject obj(get_proc_addr, lib, init);

    // register all extension classes during scene initialization phase
    obj.register_initializer([](godot::moduleinitializationlevel lvl)
    {
        if (lvl == godot::module_initialization_level_scene)
        {
            // register each extension class with the godot runtime
            godot::classdb::register_class<control_main>();
            godot::classdb::register_class<control_scene_td>();
            godot::classdb::register_class<control_scene_practice>();
            godot::classdb::register_class<control_scene_tutorial>();
            godot::classdb::register_class<control_scene_home>();
            godot::classdb::register_class<control_scene_dodge>();

            // register base pilot classes - abstract bases are handled differently
            godot::classdb::register_abstract_class<dragon_pilot_top>();  // no instantiation

            // register concrete pilot implementations
            godot::classdb::register_class<dragon_pilot_keyboard>();
            godot::classdb::register_class<dragon_pilot_joystick>();
            godot::classdb::register_class<dragon_pilot_dodge>();

            // register utility classes for animation and timing
            godot::classdb::register_class<dragon_animator>();
            godot::classdb::register_class<control_camera>();
            godot::classdb::register_class<gametimer>();
            godot::classdb::register_class<cheatsheet>();
            godot::classdb::register_class<savemanager>();
            godot::classdb::register_class<sunsetbridge>();
        }
    });

    // inform godot this extension requires scene-level services
    obj.set_minimum_library_initialization_level(godot::module_initialization_level_scene);
    return obj.init();
}

// end of file
// note: the macro system here is sophisticated c++ metaprogramming -
// once compiled, the macro expansion creates zero runtime overhead while
// providing compile-time type safety and boilerplate generation.