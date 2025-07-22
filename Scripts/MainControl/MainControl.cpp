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
    camera_ctrl = memnew(CameraControl(sub_view, enable_headset));
    camera_ctrl->set_name("CameraControl"); // set the name of the camera control node
    dragon_node->add_child(camera_ctrl); // add the camera control to the dragon node
    timer = memnew(GameTimer(camera_ctrl));
    timer->set_name("GameTimer");
    add_child(timer);

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
    if (sub_view) 
    {
        video_player = dragon_node->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<VideoStreamPlayer>("VideoStreamPlayer");
    }

    camera_ctrl->SetDragonControl(dragon_control); // set the dragon control to the camera control
    save_manager = memnew(SaveManager());
    add_child(save_manager);
    audio_player = get_parent()->get_node<AudioStreamPlayer>("AudioStreamPlayer");

    Initialize_TimerList();
    call_deferred("Start_Timer"); // postpone for one frame to ensure the scene is fully initialized and rendered
}


/**
 * @brief add events to the timer list
 */
void MainControl::Initialize_TimerList() 
{
    // test timer list
    // timer->Timer_AddEvent(0.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_CRISIS));
    // timer->Timer_AddEvent(3.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-1.0f));

    if (sub_view && video_player)
    {
        timer->Timer_AddEvent(0.0f, Callable(video_player, "play"));
    }
    timer->Timer_AddEvent(0.0f, Callable(audio_player, "play"));
    timer->Timer_AddEvent(16.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_NOT_ANIMATED)); // disable the default animations
    timer->Timer_AddEvent(16.8f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_tail_wing_close")); // the tail wing folds
    timer->Timer_AddEvent(17.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_glide")); // the tail wing is now fully extended
    timer->Timer_AddEvent(18.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_check_tail_glide", true)); // the starting position of tr_check_tail_glide
    timer->Timer_AddEvent(19.1f, Callable(dragon_animator, "Unfreeze")); // change the animation to tr_check_tail_glide
    timer->Timer_AddEvent(20.8f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DEFAULT)); // enable the default animations
    timer->Timer_AddEvent(45.0f, Callable(dragon_control, "TriggerApproaching").bind(false, get_parent()->get_node<Node3D>("Rocks/Area_Beginning/Rock_Pillar_A_01")->get_global_transform().origin + Vector3(0, 60, 0), 7.0f)); 
    timer->Timer_AddEvent(53.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // the code takes control, unavoidable to fly towards the pillar
    timer->Timer_AddEvent(53.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_glide")); 
    timer->Timer_AddEvent(58.2f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true)); // ready to hit the pillar, setting the animation to hit_cliff
    timer->Timer_AddEvent(59.3f, Callable(dragon_animator, "Unfreeze")); // hit the pillar for the first time
    timer->Timer_AddEvent(59.7f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DISABLED));
    timer->Timer_AddEvent(59.9f, Callable(dragon_control, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // unavoidable to fly towards the pillar
    timer->Timer_AddEvent(59.9f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up")); // Toothless flap his wings
    timer->Timer_AddEvent(62.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true));
    timer->Timer_AddEvent(63.2f, Callable(dragon_animator, "Unfreeze")); // hit the pillar for the second time
    timer->Timer_AddEvent(63.5f, Callable(dragon_control, "TriggerApproaching").bind(true, get_parent()->get_node<Node3D>("Rocks/Area_Beginning/Rock_Pillar_C_10")->get_global_transform().origin + Vector3(0, 200, 0), 10.0f)); // Toothless turns around and keep flying
    timer->Timer_AddEvent(63.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up"));
    timer->Timer_AddEvent(63.5f, Callable(dragon_control, "SetClearToothlessRotation").bind(true)); // set clear_pivot_rotation to true
    timer->Timer_AddEvent(65.3f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_slap_glide")); // Toothless flap my face with his ear
    timer->Timer_AddEvent(70.0f, Callable(dragon_control, "TriggerApproaching").bind(true, Vector3(-1000, 3100, -500), 10.5f)); // fly up to the sky
    timer->Timer_AddEvent(70.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up"));
    timer->Timer_AddEvent(80.5f, Callable(cheat_sheet, "Detatch")); // detatch the cheat sheet
    timer->Timer_AddEvent(82.5f, Callable(dragon_control, "SetState").bind(DragonState::STATE_FALLING)); // start to decelerate due to the stall
    timer->Timer_AddEvent(84.0f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(0, 0, -Math_PI/2), 0.5f)); // the camera starts to face downwards
    timer->Timer_AddEvent(84.0f, Callable(camera_ctrl, "SetCameraOffsetFactor").bind(1.055f));
    timer->Timer_AddEvent(85.7f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_fall")); // change the animation to tr_glide_fall
    timer->Timer_AddEvent(86.0f, Callable(dragon_animator, "SetAnimation").bind("layer_eye_shape", "po_eye_small"));
    timer->Timer_AddEvent(86.0f, Callable(camera_ctrl, "SetCameraOffsetFactor").bind(0.98f));
    timer->Timer_AddEvent(86.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-1, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(86.5f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(0, 0, Math_PI/2), 0.1f)); // start to change the camera to upwards
    timer->Timer_AddEvent(87.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(88.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(88.8f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(89.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(89.9f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(91.0f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(Math_PI/10, 0, Math_PI/2), 5.0f)); // change the camera to downwards
    timer->Timer_AddEvent(91.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(91.3f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(-Math_PI/5, 0, Math_PI/2), 5.0f)); // change the camera to downwards
    timer->Timer_AddEvent(91.4f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(Math_PI, 0, Math_PI/2), 2.5f)); // change the camera to downwards
    timer->Timer_AddEvent(91.7f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(91.7f, Callable(camera_ctrl, "SetCameraOffsetFactor").bind(1.2f));
    timer->Timer_AddEvent(92.3f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(0, 0, 0), -3.8f)); // hit Toothless's wing, and then start to rotate the camera for two rounds
    timer->Timer_AddEvent(92.5f, Callable(camera_ctrl, "SetCameraOffsetFactor").bind(1.01f));
    timer->Timer_AddEvent(93.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(93.5f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(Math_PI, 0, -Math_PI), 1.0f));
    timer->Timer_AddEvent(92.3f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(-Math_PI/2, -Math_PI/2, Math_PI)));
    timer->Timer_AddEvent(93.5f, Callable(dragon_control, "SetVelocityAngular").bind(Vector3(0.0f, 6.3f, 0.0f))); // Toothless starts to rotate unwillingly
    timer->Timer_AddEvent(93.0f, Callable(camera_ctrl, "TriggerApproachingPosition").bind(Vector3(7, 0, -7))); // the camera starts to approach Toothless
    timer->Timer_AddEvent(99.0f, Callable(camera_ctrl, "TriggerApproachingPosition").bind(Vector3(3, 0, -3)));
    timer->Timer_AddEvent(99.8f, Callable(camera_ctrl, "TriggerApproachingPosition").bind(Vector3(10, 8, -10))); // Toothless hit me with his tail
    timer->Timer_AddEvent(99.8f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(0, Math_PI, -Math_PI), 2.0f)); // the camera is spinning for one round
    timer->Timer_AddEvent(100.0f, Callable(dragon_control, "SetVelocityAngular").bind(Vector3(0, 3, 0))); // Toothless's spinning is alleviated
    timer->Timer_AddEvent(101.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(2, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(101.4f, Callable(dragon_control, "SetVelocityAngular").bind(Vector3(0, 0, 0)));
    timer->Timer_AddEvent(101.5f, Callable(camera_ctrl, "TriggerApproachingPosition").bind(Vector3(-2, 8, 6))); // Toothless hit me with his tail
    timer->Timer_AddEvent(101.5f, Callable(camera_ctrl, "TriggerApproachingAngle").bind(Vector3(0, -Math_PI/2, -Math_PI/2), 1.0f)); // the camera is now facing downwards
    timer->Timer_AddEvent(101.5f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(0,-Math_PI/2, -Math_PI/2))); // straightly dive downwards
    timer->Timer_AddEvent(101.5f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(102.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(2, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(103.0f, Callable(camera_ctrl, "TriggerApproachingPosition").bind(Vector3(0, 0, 0)));
    timer->Timer_AddEvent(103.0f, Callable(camera_ctrl, "SetCameraOffsetFactor").bind(1.0f));
    timer->Timer_AddEvent(106.5f, Callable(camera_ctrl, "GrabSaddle")); // grab the saddle
    timer->Timer_AddEvent(106.8f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.5f)); // Toothless opens his mouth
    timer->Timer_AddEvent(108.7f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_dive")); // change the animation to po_dive
    timer->Timer_AddEvent(113.7f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_crisis"));
    timer->Timer_AddEvent(113.8f, Callable(dragon_control, "TriggerApproaching").bind(false, get_parent()->get_node<Node3D>("Rocks/Area_Final/Rock_Pillar_E_01")->get_global_transform().origin + Vector3(0, 15, 0), 9.0f)); // glide diagonal downwards
    timer->Timer_AddEvent(113.8f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(113.7f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(-0.3f, -Math_PI/2 + 0.3f, -Math_PI/2 -0.3f)));
    timer->Timer_AddEvent(113.8f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(0.6f, -Math_PI/2 - 0.6f, -Math_PI/2 + 0.6f)));
    timer->Timer_AddEvent(113.9f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(0, -Math_PI/2, Math_PI/8))); // glide diagonal downwards
    timer->Timer_AddEvent(116.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-1, 0.33f)); // Toothless opens his mouth
    timer->Timer_AddEvent(120.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_tail_wing_close"));
    timer->Timer_AddEvent(120.1f, Callable(dragon_control, "SetTargetRotation").bind(Vector3(0,-Math_PI/2, 0))); // glide diagonal downwards
    // timer->Timer_AddEvent(120.5f, Callable(camera_ctrl, "SetCameraStabilized").bind(true));
    timer->Timer_AddEvent(121.7f, Callable(dragon_control, "SetState").bind(DragonState::STATE_CRISIS)); // fully retrieve the control and the tail wing is fully extended
    timer->Timer_AddEvent(121.7f, Callable(dragon_animator, "SetAnimation_Mouth").bind(4, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(121.7f, Callable(dragon_control, "SetStatus_Deferred").bind(Array{0}, 300.0f));
    timer->Timer_AddEvent(129.0f, Callable(dragon_control, "SetState").bind(DragonState::STATE_ROLLING)); // start getting to the position of upside down
    timer->Timer_AddEvent(134.8f, Callable(dragon_control, "SetState").bind(DragonState::STATE_DISABLED)); // successfully traversed the crisis and set the velocity to horizontal
    timer->Timer_AddEvent(142.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_celebrate")); // change the animation to celebrate
    timer->Timer_AddEvent(143.5f, Callable(dragon_animator, "SetAnimation").bind("layer_eye_shape", "po_eye_big"));
    timer->Timer_AddEvent(143.5f, Callable(dragon_animator, "SetAnimation").bind("layer_mouth", "tr_glide_celebrate"));
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
        timer->Timer_ForceSetTime(time_elapsed);
        if (sub_view && video_player)
        {
            video_player->set_stream_position(time_elapsed);
        }
        dragon_control->SetStatus(data_all); 
        dragon_control->time_to_target = 0.1f;
        camera_ctrl->SetCameraStabilized(false);
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