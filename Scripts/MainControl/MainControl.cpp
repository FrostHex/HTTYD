#include "MainControl.h"
#include "DragonControlKeyboard.h"
#include "DragonControlJoystick.h"
#include "CameraControl.h"

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
#include <godot_cpp/classes/timer.hpp>

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

    Node *dragon_node = get_parent()->get_node<Node>("Dragon");
    cheat_sheet = dragon_node->get_node<CheatSheet>("CheatSheet");
    dragon_animator = get_parent()->get_node<Node>("Dragon")->get_node<DragonAnimator>("DragonAnimator");
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
    timer = memnew(GameTimer(camera_ctrl));
    add_child(timer);
    save_manager = memnew(SaveManager());
    add_child(save_manager);
    audio_player = get_parent()->get_node<AudioStreamPlayer>("AudioStreamPlayer");
    video_player = dragon_node->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<VideoStreamPlayer>("VideoStreamPlayer");

    Initialize_TimerList();
    call_deferred("Start_Timer"); // postpone for one frame to ensure the scene is fully initialized and rendered
}


void MainControl::Initialize_TimerList() 
{
    // test timer list
    // timer->Timer_AddEvent(0.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_NOT_ANIMATED)); // 14.8 disable the default animations
    
    timer->Timer_AddEvent(0.0f, Callable(audio_player, "play"));
    timer->Timer_AddEvent(0.0f, Callable(video_player, "play"));
    timer->Timer_AddEvent(16.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_NOT_ANIMATED)); // disable the default animations
    timer->Timer_AddEvent(16.8f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_tail_wing_close")); // the tail wing folds
    timer->Timer_AddEvent(17.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_glide")); // the tail wing is now fully extended
    timer->Timer_AddEvent(18.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_check_tail_glide", true)); // the starting position of tr_check_tail_glide
    timer->Timer_AddEvent(19.1f, Callable(dragon_animator, "Unfreeze")); // change the animation to tr_check_tail_glide
    timer->Timer_AddEvent(20.8f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DEFAULT)); // enable the default animations
    timer->Timer_AddEvent(45.0f, Callable(dragon_control, "TriggerApproaching").bind(get_parent()->get_node<Node3D>("Rocks/Area_Beginning/Rock_Pillar_A_01")->get_global_transform().origin + Vector3(0, 60, 0), 7.0f)); 
    timer->Timer_AddEvent(53.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // the code takes control, unavoidable to fly towards the pillar
    timer->Timer_AddEvent(53.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_glide")); 
    timer->Timer_AddEvent(58.2f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true)); // ready to hit the pillar, setting the animation to hit_cliff
    timer->Timer_AddEvent(59.3f, Callable(dragon_animator, "Unfreeze")); // hit the pillar
    timer->Timer_AddEvent(59.7f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DISABLED));
    timer->Timer_AddEvent(59.9f, Callable(dragon_control, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // unavoidable to fly towards the pillar
    timer->Timer_AddEvent(59.9f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up")); 
    timer->Timer_AddEvent(62.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true));
    timer->Timer_AddEvent(63.2f, Callable(dragon_animator, "Unfreeze")); // hit the pillar
    // 1'04.0 Toothless turns around and keep flying
    // 1'05.3 Toothless flap my face with his ear
    timer->Timer_AddEvent(80.5f, Callable(cheat_sheet, "Detatch")); // detatch the cheat sheet
    // 1'22.5 start to decelerate due to the stall
    // 1'25.0 the camera is now facing downwards
    // 1'25.5 change the animation to tr_glide_fall
    // 1'25.8 speed is now reduced to 0, and start falling
    // 1'27.0 start to change the camera to upwards within 1 second
    // 1'28.0 Toothless opens his mouth to roar
    // 1'29.0 Toothless opens his mouth to roar
    // 1'30.5 Toothless opens his mouth to roar
    // 1'30.8 change the camera to downwards within 0.4 seconds
    // 1'32.0 hit Toothless's wing, and then start to rotate the camera for two rounds
    // 1'33.2 Toothless opens his mouth to roar
    // 1'35.0 Toothless starts to rotate unwillingly
    // 1'39.5 Toothless hit me with his tail and the camera is spinning for one round
    // 1'41.0 the camera is now facing downwards and approaching to Toothless, and Toothless's spinning is alleviated
    // 1'44.3 grab the saddle
    // 1'46.5 sit back on the saddle
    // 1'50.0 straightly dive downwards
    // 1'53.5 glide diagonal downwards
    // 2'01.7 the tail wing is now fully extended
    // 2'02.0 fully retrieve the control
    // 2'08.7 start getting to the position of upside down
    // 2'09.7 finish the spinning
    // 2'10.5 retrieve the control
    // 2'14.8 successfully traversed the crisis
    // 2'22.5 change the animation to celebrate
    timer->Timer_AddEvent(143.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DISABLED)); // 2'23.0 disable the control
}


/**
 * @brief starts the timer with a delay to ensure the video is fully loaded
 */
void MainControl::Start_Timer()
{
    if (timer) 
    {
        Timer* delay_timer = memnew(Timer);
        add_child(delay_timer);
        delay_timer->set_wait_time(1.0f);
        delay_timer->set_one_shot(true);
        delay_timer->connect("timeout", Callable(timer, "Timer_Resume"));
        delay_timer->connect("timeout", Callable(delay_timer, "queue_free"));
        delay_timer->start();
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
    if (event->is_action_pressed("save_state")) 
    {
        Dictionary data_all;
        data_all["time"] = timer->Timer_GetTimeElapsed();
        Dictionary data_dragon = dragon_control->GetStatus();
        Array keys = data_dragon.keys();
        for (int i = 0; i < keys.size(); i++) 
        {
            Variant key = keys[i];
            Variant value = data_dragon[key];
            data_all[key] = value;
        }
        save_manager->State_Save(data_all);
    }
    if (event->is_action_pressed("load_state")) 
    {
        Dictionary data_all = save_manager->State_Load();
        float time_elapsed = data_all["time"];
        audio_player->seek(time_elapsed);
        timer->Timer_Reset();
        Initialize_TimerList();
        timer->Timer_Set(time_elapsed);
        video_player->set_stream_position(time_elapsed);
        dragon_control->SetStatus(data_all); 
        dragon_control->time_to_target = 0.1f;
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
    ClassDB::bind_method(D_METHOD("Start_Timer"), &MainControl::Start_Timer);
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
            godot::ClassDB::register_class<SaveManager>();
        }
    });
    obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    return obj.init();
}