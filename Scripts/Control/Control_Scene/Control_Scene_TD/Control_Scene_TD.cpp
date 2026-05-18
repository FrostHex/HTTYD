#include "Control_Scene_TD.h"
#include "Control_Main.h"
#include "Dragon_Pilot_Keyboard.h"
#include "Dragon_Pilot_Joystick.h"
#include "Settings.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp> // memnew
#include <godot_cpp/classes/input_event.hpp> 
#include <godot_cpp/classes/input_event_action.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp> // for get_tree()
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/window.hpp> // for Window class
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/environment.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>


using namespace godot;


/**
 * @brief default constructor for Godot registration
 */
Control_Scene_TD::Control_Scene_TD()
{
}

/**
 * @brief destructor
 */
Control_Scene_TD::~Control_Scene_TD()
{
}


void Control_Scene_TD::_process(double delta)
{
	_process_top(delta);
}

/**
 * @brief called when the node and its children are initialized
 * @note 1. when open a scene containing this node in Godot Engine (editor mode, not running)
 * @note 2. when the game starts running and the this node is contained
 */
void Control_Scene_TD::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only proceed when the game is running
    {
        return;
    }

    Control_Scene_Top::_ready();

    // Get reference to Control_Main
    SceneTree *tree = get_tree();
    if (tree) 
    {
        Window *root = tree->get_root();
        if (root) 
        {
            control_main = Object::cast_to<Control_Main>(root->get_node_or_null(NodePath("Main/Control_Main")));
            if (!control_main) 
            {
                UtilityFunctions::printerr("Control_Scene_TD: Could not find Control_Main at Main/Control_Main");
                return;
            }
        }
    }

    Node *dragon_node = get_parent()->get_node<Node>("Dragon");
    cheat_sheet = dragon_node->get_node<CheatSheet>("CheatSheet");
    dragon_animator = get_parent()->get_node<Node>("Dragon")->get_node<Dragon_Animator>("Dragon_Animator");
	ctrl_camera = tree->get_root()->get_node<Control_Camera>("Main/Control_Main/Control_Camera");
	ctrl_camera->call_deferred("Initialize");
    timer = get_node<GameTimer>("GameTimer");
    timer->Initialize(ctrl_camera);

    if (Settings::GetSingleton()->GetValEnableHeadset()) 
    {
        // memnew is "new" in Godot C++, which dynamically allocates memory for the object
        // memnew() creates an instance of Dragon_Pilot_Joystick and returns a pointer to it
        dragon_pilot = memnew(Dragon_Pilot_Joystick);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot)); // add the dragon control to the dragon node
        dragon_pilot->set_name("Dragon_Pilot_Joystick"); // set the name of the dragon control node
    }
    else
    {
        dragon_pilot = memnew(Dragon_Pilot_Keyboard);
        dragon_node->add_child(dynamic_cast<Node*>(dragon_pilot));
        dragon_pilot->set_name("Dragon_Pilot_Keyboard"); // set the name of the dragon control node
    }
    if (Settings::GetSingleton()->GetValSubView() && Settings::GetSingleton()->GetValDebugInfo()) 
    {
        video_player = dragon_node->get_node<Node>("SubViewportContainer")->get_node<Node>("SubViewport")->get_node<VideoStreamPlayer>("VideoStreamPlayer");
    }

    ctrl_camera->SetDragon_Pilot_(dragon_pilot); // set the dragon control to the camera control
    save_manager = get_node<SaveManager>("SaveManager");
    audio_player = get_parent()->get_node<AudioStreamPlayer>("AudioStreamPlayer");

    // connect achievement area signals (if present).
    td_area_1 = get_parent()->get_node_or_null(NodePath("TD_Area_1"));
    td_area_2 = get_parent()->get_node_or_null(NodePath("TD_Area_2"));
    if (td_area_1) {
        td_area_1->connect("body_entered", Callable(this, "_on_td_area_1_body_entered"));
    } else {
        UtilityFunctions::push_warning("Control_Scene_TD: TD_Area_1 not found in Scene_TD.tscn root. Achievements area1 disabled.");
    }
    if (td_area_2) {
        td_area_2->connect("body_entered", Callable(this, "_on_td_area_2_body_entered"));
    } else {
        UtilityFunctions::push_warning("Control_Scene_TD: TD_Area_2 not found in Scene_TD.tscn root. Achievements area2 disabled.");
    }

    Initialize_TimerList();
    call_deferred("Start_Timer"); // postpone for one frame to ensure the scene is fully initialized and rendered

    // Test Directly
    // dragon_pilot->SetState(DragonState::STATE_CRISIS); // start in crisis state to trigger the first cutscene
}


/**
 * @brief add events to the timer list
 */
void Control_Scene_TD::Initialize_TimerList() 
{
    if (Settings::GetSingleton()->GetValSubView() && Settings::GetSingleton()->GetValDebugInfo() && video_player)
    {
        timer->Timer_AddEvent(0.0f, Callable(video_player, "play"));
    }
    timer->Timer_AddEvent(0.0f, Callable(audio_player, "play"));
    if (Settings::GetSingleton()->GetValAutoSaveState())
    {
        timer->Timer_AddEvent(3.0f, Callable(this, "AutoSave"));
        timer->Timer_AddEvent(112.5f, Callable(this, "AutoSave"));
    }
    timer->Timer_AddEvent(16.0f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_NOT_ANIMATED)); // disable the default animations
    timer->Timer_AddEvent(16.8f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_tail_wing_close")); // the tail wing folds
    timer->Timer_AddEvent(17.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_glide")); // the tail wing is now fully extended
    timer->Timer_AddEvent(18.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_check_tail_glide", true)); // the starting position of tr_check_tail_glide
    timer->Timer_AddEvent(19.1f, Callable(dragon_animator, "Unfreeze")); // change the animation to tr_check_tail_glide
    timer->Timer_AddEvent(20.8f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_DEFAULT)); // enable the default animations
    timer->Timer_AddEvent(47.0f, Callable(dragon_pilot, "TriggerApproaching").bind(false, get_parent()->get_node<Node3D>("Rocks/Area_Beginning/Rock_Pillar_A_01")->get_global_transform().origin + Vector3(0, 60, 0), 5.0f)); 
    timer->Timer_AddEvent(47.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_glide")); 
    timer->Timer_AddEvent(47.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_glide"));
    timer->Timer_AddEvent(53.0f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // the code takes control, unavoidable to fly towards the pillar
    timer->Timer_AddEvent(53.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_glide")); 
    timer->Timer_AddEvent(58.2f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true)); // ready to hit the pillar, setting the animation to hit_cliff
    timer->Timer_AddEvent(59.3f, Callable(dragon_animator, "Unfreeze")); // hit the pillar for the first time
    timer->Timer_AddEvent(59.7f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_DISABLED));
    timer->Timer_AddEvent(59.9f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_HIT_CLIFF)); // unavoidable to fly towards the pillar
    timer->Timer_AddEvent(59.9f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up")); // Toothless flap his wings
    timer->Timer_AddEvent(62.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_hit_glide", true));
    timer->Timer_AddEvent(63.2f, Callable(dragon_animator, "Unfreeze")); // hit the pillar for the second time
    timer->Timer_AddEvent(63.5f, Callable(dragon_pilot, "TriggerApproaching").bind(true, get_parent()->get_node<Node3D>("Rocks/Area_Beginning/Rock_Pillar_C_10")->get_global_transform().origin + Vector3(0, 200, 0), 10.0f)); // Toothless turns around and keep flying
    timer->Timer_AddEvent(63.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up"));
    timer->Timer_AddEvent(63.5f, Callable(dragon_pilot, "SetClearToothlessRotation").bind(true)); // set clear_pivot_rotation to true
    timer->Timer_AddEvent(65.2f, Callable(dragon_pilot, "SetClearToothlessRotation").bind(false));
    timer->Timer_AddEvent(65.3f, Callable(ctrl_camera, "ReparentCamera").bind(NodePath("SpeciesSlot/ToothlessRoot/Sockets"))); 
    timer->Timer_AddEvent(65.3f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_slap_glide")); // Toothless flap my face with his ear
    timer->Timer_AddEvent(69.5f, Callable(ctrl_camera, "ReparentCamera").bind(NodePath("SpeciesSlot/ToothlessRoot/Sockets/Socket_Back_Mount/Socket_Back"))); 
    timer->Timer_AddEvent(70.0f, Callable(dragon_pilot, "TriggerApproaching").bind(true, Vector3(-1000, 3150, -500), 10.5f)); // fly up to the sky
    timer->Timer_AddEvent(70.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "lo_up"));
    timer->Timer_AddEvent(70.1f, Callable(dragon_pilot, "SetClearToothlessRotation").bind(true));
    timer->Timer_AddEvent(80.5f, Callable(cheat_sheet, "Detatch")); // detatch the cheat sheet
    timer->Timer_AddEvent(82.5f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_FALLING)); // start to decelerate due to the stall
    timer->Timer_AddEvent(84.0f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(0, 0, -Math_PI/2), 0.5f)); // the camera starts to face downwards
    timer->Timer_AddEvent(84.0f, Callable(ctrl_camera, "SetCameraOffsetFactor").bind(1.035f));
    timer->Timer_AddEvent(85.7f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_fall")); // change the animation to tr_glide_fall
    timer->Timer_AddEvent(86.0f, Callable(dragon_animator, "SetAnimation").bind("layer_eye_shape", "po_eye_small"));
    timer->Timer_AddEvent(86.0f, Callable(ctrl_camera, "SetCameraOffsetFactor").bind(0.99f));
    timer->Timer_AddEvent(86.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-1, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(86.5f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(0, Math_PI/3, Math_PI/2), 0.1f)); // start to change the camera to upwards
    timer->Timer_AddEvent(87.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(88.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(88.8f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(89.4f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(89.9f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(91.0f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(Math_PI/10, Math_PI/3, Math_PI/2), 5.0f)); // change the camera to downwards
    timer->Timer_AddEvent(91.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-3, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(91.3f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(-Math_PI/5, Math_PI/3, Math_PI/2), 5.0f)); // change the camera to downwards
    timer->Timer_AddEvent(91.4f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(Math_PI, Math_PI/3, Math_PI/2), 2.5f)); // change the camera to downwards
    timer->Timer_AddEvent(91.7f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(91.7f, Callable(ctrl_camera, "SetCameraOffsetFactor").bind(1.2f));
    timer->Timer_AddEvent(92.3f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(0, Math_PI/2, 0), -3.8f)); // hit Toothless's wing, and then start to rotate the camera for two rounds
    timer->Timer_AddEvent(92.5f, Callable(ctrl_camera, "SetCameraOffsetFactor").bind(1.01f));
    timer->Timer_AddEvent(93.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(93.5f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(Math_PI, Math_PI/4, -Math_PI), 1.0f));
    timer->Timer_AddEvent(92.3f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(-Math_PI/2, -Math_PI/2, Math_PI)));
    timer->Timer_AddEvent(93.5f, Callable(dragon_pilot, "SetVelocityAngular").bind(Vector3(0.0f, 6.3f, 0.0f))); // Toothless starts to rotate unwillingly
    timer->Timer_AddEvent(93.0f, Callable(ctrl_camera, "TriggerApproachingPosition").bind(Vector3(7.0f, 0.65f, -7.0f))); // the camera starts to approach Toothless
    timer->Timer_AddEvent(99.0f, Callable(ctrl_camera, "TriggerApproachingPosition").bind(Vector3(3.0f, 0.65f, -2.5f))); 
    timer->Timer_AddEvent(99.8f, Callable(ctrl_camera, "TriggerApproachingPosition").bind(Vector3(10, 8, -10))); // Toothless hit me with his tail
    timer->Timer_AddEvent(99.8f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(0, Math_PI, -Math_PI), 2.0f)); // the camera is spinning for one round
    timer->Timer_AddEvent(100.0f, Callable(dragon_pilot, "SetVelocityAngular").bind(Vector3(0, 3, 0))); // Toothless's spinning is alleviated
    timer->Timer_AddEvent(101.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(2, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(101.4f, Callable(dragon_pilot, "SetVelocityAngular").bind(Vector3(0, 0, 0)));
    timer->Timer_AddEvent(101.5f, Callable(ctrl_camera, "TriggerApproachingPosition").bind(Vector3(-2, 8, 6)));
    timer->Timer_AddEvent(101.5f, Callable(ctrl_camera, "TriggerApproachingAngle").bind(Vector3(0, -Math_PI/2, -Math_PI/2), 1.0f)); // the camera is now facing downwards
    timer->Timer_AddEvent(101.5f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(0,-Math_PI/2, -Math_PI/2))); // straightly dive downwards
    timer->Timer_AddEvent(101.5f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.0f)); // Toothless opens his mouth to roar
    timer->Timer_AddEvent(102.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(2, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(103.0f, Callable(ctrl_camera, "TriggerApproachingPosition").bind(Vector3(0.0f, 0.0f, 0.8f)));
    timer->Timer_AddEvent(103.0f, Callable(ctrl_camera, "SetCameraOffsetFactor").bind(1.0f));
    timer->Timer_AddEvent(105.0f, Callable(ctrl_camera, "GrabSaddle")); // grab the saddle
    timer->Timer_AddEvent(106.8f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-2, 0.5f)); // Toothless opens his mouth
    timer->Timer_AddEvent(108.7f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_dive")); // change the animation to po_dive
    timer->Timer_AddEvent(113.5f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(-0.3f, -Math_PI/2 + 0.3f, -Math_PI/2 -0.3f)));
    timer->Timer_AddEvent(113.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_crisis"));
    timer->Timer_AddEvent(113.6f, Callable(dragon_pilot, "TriggerApproaching").bind(false, get_parent()->get_node<Node3D>("Rocks/Area_Final/Rock_Pillar_E_01")->get_global_transform().origin + Vector3(0, 15, 0), 9.5f)); // glide diagonal downwards
    timer->Timer_AddEvent(113.6f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(0.6f, -Math_PI/2 - 0.6f, -Math_PI/2 + 0.6f)));
    timer->Timer_AddEvent(113.7f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(0, -Math_PI/2, Math_PI/8))); // glide diagonal downwards
    timer->Timer_AddEvent(114.0f, Callable(dragon_animator, "SetAnimation_Mouth").bind(3, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(116.3f, Callable(dragon_animator, "SetAnimation_Mouth").bind(-1, 0.33f)); // Toothless opens his mouth
    timer->Timer_AddEvent(120.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_tail_wing_close"));
    timer->Timer_AddEvent(120.1f, Callable(dragon_pilot, "SetTargetRotation").bind(Vector3(0,-Math_PI/2, 0))); // glide diagonal downwards
    timer->Timer_AddEvent(121.7f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_CRISIS)); // fully retrieve the control and the tail wing is fully extended
    timer->Timer_AddEvent(121.7f, Callable(dragon_animator, "SetAnimation_Mouth").bind(4, 1.0f)); // Toothless closes his mouth
    timer->Timer_AddEvent(121.7f, Callable(dragon_pilot, "SetStatus_Deferred").bind(Array{0}, 300.0f));
    timer->Timer_AddEvent(113.8f, Callable(ctrl_camera, "SetCameraStabilized").bind(true)); 
    if (Settings::GetSingleton()->GetValAutoRoll())
    {
        timer->Timer_AddEvent(129.0f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_ROLLING)); // automatically start getting to the position of upside down
    }
    timer->Timer_AddEvent(137.0f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_DISABLED)); // successfully traversed the crisis and set the velocity to horizontal
    timer->Timer_AddEvent(137.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "po_glide")); 
    timer->Timer_AddEvent(137.0f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_tail", "po_glide"));
    timer->Timer_AddEvent(142.5f, Callable(dragon_animator, "SetAnimation").bind("layer_wing_main", "tr_glide_celebrate")); // change the animation to celebrate
    timer->Timer_AddEvent(143.5f, Callable(dragon_animator, "SetAnimation").bind("layer_eye_shape", "po_eye_big"));
    timer->Timer_AddEvent(143.5f, Callable(dragon_animator, "SetAnimation").bind("layer_mouth", "tr_glide_celebrate"));
    timer->Timer_AddEvent(147.5f, Callable(dragon_pilot, "SetState").bind(DragonState::STATE_DISABLED));
    timer->Timer_AddEvent(147.5f, Callable(dragon_animator, "SetAnimation_Weight").bind("add_shake", 0.0f));
    timer->Timer_AddEvent(147.5f, Callable(this, "TakeRest")); 
}


void Control_Scene_TD::TakeRest()
{
    Node3D* rocks = get_parent()->get_node<Node3D>("Rocks");
    Node3D* fog = get_parent()->get_node<Node3D>("Fog_Volume");
    Node3D* dragon_node = get_parent()->get_node<Node3D>("Dragon");
    Node* sky_node = get_parent()->get_node<Node>("Sky3D");
    if (rocks && fog && dragon_node && sky_node)
    {
        rocks->set_visible(false); // hide the rocks
        fog->set_visible(false); // hide the fog
        // sky_node->set("current_time", 19.25f); // set sky to evening time
        // sky_node->set("camera_exposure", 1.75f);
        sky_node->queue_free();
        // get new sky from res://Scenes/Sky3d_Sunset.tscn
        Ref<PackedScene> sky_scene_sunset = ResourceLoader::get_singleton()->load("res://Scenes/Sky3d_Sunset.tscn");
        Node* sky_node_sunset = nullptr;
        if (sky_scene_sunset.is_valid())
        {
            sky_node_sunset = sky_scene_sunset->instantiate();
            get_parent()->add_child(sky_node_sunset);
            sky_node_sunset->set_name("Sky3D_Sunset");
        }
        WorldEnvironment *sky_env = Object::cast_to<WorldEnvironment>(sky_node_sunset);
        if (sky_env)
        {
            Ref<Environment> env = sky_env->get_environment();
            if (env.is_valid())
            {
                env->set("adjustment_brightness", 0.85f);
                env->set("adjustment_contrast", 1.075f);
                env->set("adjustment_saturation", 0.8f);
            }
        }
        dragon_node->set_position(Vector3(1025.584f, 6.443f, -813.842f));
        dragon_node->set_rotation(Vector3(0.0f, Math::deg_to_rad(-90.0f), Math::deg_to_rad(14.8f)));
    }
    dragon_animator->SetAnimation("layer_wing_main", "po_rest");
    dragon_animator->SetAnimation("layer_mouth", "po_mouth_close");
    ctrl_camera->camera_main->reparent(get_parent());
    ctrl_camera->camera_main->set_position(Vector3(1027.504f, 6.733f, -814.004f));
    ctrl_camera->camera_main->set_rotation(Vector3(0.0f, Math::deg_to_rad(-111.0f), 0.0f));
    ctrl_camera->SetFreeCamera(true);

    if (control_main)
    {
        control_main->call_deferred("AttachSunshineClouds", get_parent()->get_name(), false);
    }

    // complete this Test Drive and update badge.
    _update_badge_on_completion();
}

void Control_Scene_TD::AutoSave()
{
    if (save_manager && timer && dragon_pilot) 
    {
        Dictionary data_all;
        data_all["time"] = timer->Timer_GetTimeElapsed();
        Dictionary data_dragon = dragon_pilot->GetStatus();
        Array keys = data_dragon.keys();
        for (int i = 0; i < keys.size(); i++) 
        {
            Variant key = keys[i];
            Variant value = data_dragon[key];
            data_all[key] = value;
        }
        save_manager->State_Save(data_all);
        UtilityFunctions::print("Auto-saved at time: ", timer->Timer_GetTimeElapsed());
    }
}


/**
 * @brief starts the timer with a delay to ensure the video is fully loaded
 */
void Control_Scene_TD::Start_Timer()
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
void Control_Scene_TD::_input(const Ref<InputEvent> &event) 
{
    _input_top(event);

    if (event->is_action_pressed("save_state")) 
    {
        Dictionary data_all;
        data_all["time"] = timer->Timer_GetTimeElapsed();
        Dictionary data_dragon = dragon_pilot->GetStatus();
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
        if (Settings::GetSingleton()->GetValSubView() && Settings::GetSingleton()->GetValDebugInfo() && video_player)
        {
            video_player->set_stream_position(time_elapsed);
        }
        dragon_pilot->SetStatus(data_all); 
        dragon_pilot->time_to_target = 0.1f;
        ctrl_camera->SetCameraStabilized(false);
        used_load_state = true;
        // after loading a save, clear only Area2 trigger state.
        if (visited_area_2) 
        {
            UtilityFunctions::print("[TD] load_state: clearing Area2 visited flag");
        }
        visited_area_2 = false;
    }
}


/**
 * @brief bind methods and properties to the Godot engine
 * @note if _bind_methods() is empty, it can still work, but the methods cannot be called in GDScript or C# or the Inspector
 * @note call ClassDB::bind_method() to expose methods to Godot in order to be used in GDScript or C#
 * @note the first line is the setter method, the second line is the getter method
 * @note &Control_Scene_TD::SetValEnableHeadset is the method pointer, which points to the actual method
 * @note this enables the method to be called like "obj.SetValEnableHeadset(true)" in GDScript or C#
 * @note call ADD_PROPERTY() to register properties to Godot
 * @note the second and third parameters are names of the binded getters and setters
 * @note after adding the property, it can be accessed in the Inspector of Godot Engine
 * @note the displayed name in the Inspector is "Enable Headset" and the type is boolean
 */
void Control_Scene_TD::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Start_Timer"), &Control_Scene_TD::Start_Timer);
    ClassDB::bind_method(D_METHOD("TakeRest"), &Control_Scene_TD::TakeRest);
    ClassDB::bind_method(D_METHOD("AutoSave"), &Control_Scene_TD::AutoSave);
    ClassDB::bind_method(D_METHOD("_on_td_area_1_body_entered", "body"), &Control_Scene_TD::_on_td_area_1_body_entered);
    ClassDB::bind_method(D_METHOD("_on_td_area_2_body_entered", "body"), &Control_Scene_TD::_on_td_area_2_body_entered);
}

// area signal callback: mark when Toothless (RigidBody3D in Dragon.tscn) enters the area.
void Control_Scene_TD::_on_td_area_1_body_entered(Node* body)
{
    if (!body) return;
    // count only when the main dragon body enters: RigidBody3D named "Dragon".
    if (Object::cast_to<RigidBody3D>(body) && body->get_name() == StringName("Dragon")) 
    {
        visited_area_1 = true;
        UtilityFunctions::print("[TD] Area1 triggered by ", body->get_name());
    }
}

void Control_Scene_TD::_on_td_area_2_body_entered(Node* body)
{
    if (!body) return;
    if (Object::cast_to<RigidBody3D>(body) && body->get_name() == StringName("Dragon")) 
    {
        visited_area_2 = true;
        UtilityFunctions::print("[TD] Area2 triggered by ", body->get_name());
    }
}

// update badge on completion: set to 1 if badge < 1; set to 2 if both areas were visited and badge < 2.
void Control_Scene_TD::_update_badge_on_completion()
{
    if (!control_main) return;
    int current_badge = Settings::GetSingleton()->GetValBadge();
    int target_badge = current_badge;
    if (current_badge < 1) 
    {
        target_badge = 1;
    }
    if (visited_area_1 && visited_area_2 && current_badge < 2) 
    {
        target_badge = 2;
    }
    // extra rule: if load_state was never used and both areas were triggered, set to 3.
    if (!used_load_state && visited_area_1 && visited_area_2 && current_badge < 3) 
    {
        target_badge = 3;
    }
    if (target_badge != current_badge) 
    {
        Settings::GetSingleton()->SetValBadge(target_badge);
        // UtilityFunctions::print("Badge updated to ", target_badge);
    }
}