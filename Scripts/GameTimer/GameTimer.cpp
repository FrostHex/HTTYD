#include "GameTimer.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


/**
 * @brief default constructor (for Godot registration)
 */
GameTimer::GameTimer(): camera_control(nullptr), time_elapsed(0.0f), timer_paused(false), id_event_next(1)
{
}


/**
 * @brief constructor
 */
GameTimer::GameTimer(CameraControl* camera_control): camera_control(camera_control), time_elapsed(0.0f), timer_paused(false), id_event_next(1)
{
}


/**
 * @brief destructor
 */
GameTimer::~GameTimer() 
{
}


/**
 * @brief bind methods to Godot
 */
void GameTimer::_bind_methods() 
{
    // register methods to Godot
    ClassDB::bind_method(D_METHOD("Timer_AddEvent", "seconds", "callback"), &GameTimer::Timer_AddEvent);
    ClassDB::bind_method(D_METHOD("Timer_AddEventSinceNow", "seconds", "callback"), &GameTimer::Timer_AddEventSinceNow);
    ClassDB::bind_method(D_METHOD("Timer_Reset"), &GameTimer::Timer_Reset);
    ClassDB::bind_method(D_METHOD("Timer_Pause"), &GameTimer::Timer_Pause);
    ClassDB::bind_method(D_METHOD("Timer_Resume"), &GameTimer::Timer_Resume);
    ClassDB::bind_method(D_METHOD("Timer_GetTimeElapsed"), &GameTimer::Timer_GetTimeElapsed);
}


/**
 * @brief reset the timer
 */
void GameTimer::_ready() 
{
    if (Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        return;
    }
    Timer_Reset();
    audio_player = get_parent()->get_parent()->get_node<AudioStreamPlayer>("AudioStreamPlayer");
}


/**
 * @brief Add a timer event
 * @param seconds the time to trigger the event
 * @param callback The callback function to trigger
 * @return Event ID, can be used to delete the event
 */
int GameTimer::Timer_AddEvent(float seconds, Callable callback) 
{
    int id_event = id_event_next++;
    event_queue.push(TimerEvent(seconds, callback, id_event));
    // UtilityFunctions::print("Added timer event ID ", id_event, " for ", seconds, " seconds (will trigger at ", seconds, ")");
    set_physics_process(true);
    return id_event;
}


/**
 * @brief Add a timer event
 * @param seconds the future time (since now) to trigger the event
 * @param callback The callback function to trigger
 * @return Event ID, can be used to delete the event
 */
int GameTimer::Timer_AddEventSinceNow(float seconds, Callable callback) 
{
    int id_event = id_event_next++;
    event_queue.push(TimerEvent(time_elapsed + seconds, callback, id_event));
    // UtilityFunctions::print("Added timer event ID ", id_event, " for ", seconds, " seconds (will trigger at ", time_elapsed + seconds, ")");
    set_physics_process(true);
    return id_event;
}


/**
 * @brief frame update function
 * @param delta time since last frame (seconds)
 */
void GameTimer::_physics_process(double delta) 
{
    if (timer_paused) 
    {
        return;
    }

    time_elapsed = audio_player->get_playback_position(); // use the audio timer for time tracking

    if (camera_control)
    {
        camera_control->time_elapsed = String::num(time_elapsed, 1);
    }

    while (!event_queue.empty()) // process all due events
    {
        const TimerEvent& event_next = event_queue.top();
        if (event_next.time_trigger > time_elapsed) 
        {
            break;
        }
        event_next.callback.call();
        if (camera_control) 
        {
            camera_control->info_debug = "Event ID: " + String::num_int64(event_next.id) + " at " + String::num(time_elapsed);
        }
        // UtilityFunctions::print("Event ID ", event_next.id, " triggered at ", time_elapsed, " seconds");
        event_queue.pop();
    }
    if (event_queue.empty())
    {
        set_physics_process(false);
    }
}


/**
 * @brief Reset the timer and all events and pause it
 */
void GameTimer::Timer_Reset() 
{
    this->Timer_Pause();
    time_elapsed = 0.0f;
    while (!event_queue.empty()) // clear event queue
    {
        event_queue.pop();
    }
    // UtilityFunctions::print("Timer Timer_Reset");
    set_physics_process(false);
}


void GameTimer::Timer_Set(float time)
{
    time_elapsed = time;
    while (!event_queue.empty()) // remove all events with time < current time
    {
        const TimerEvent& event_next = event_queue.top();
        if (event_next.time_trigger >= time_elapsed) 
        {
            break;
        }
        event_queue.pop();
    }
    Timer_Resume();
}

/**
 * @brief pause the timer
 */
void GameTimer::Timer_Pause() 
{
    timer_paused = true;
    // UtilityFunctions::print("Timer Timer_Paused at ", time_elapsed, " seconds");
}


/**
 * @brief resume the timer
 */
void GameTimer::Timer_Resume() 
{
    timer_paused = false;
    // UtilityFunctions::print("Timer Timer_Resumed at ", time_elapsed, " seconds");
    if (!event_queue.empty())
    {
        set_physics_process(true);
    }
}


/**
 * @brief get the elapsed time
 * @return elapsed time (seconds)
 */
float GameTimer::Timer_GetTimeElapsed() const 
{
    return time_elapsed;
}


void GameTimer::Timer_ForceSetTime(float time)
{
    time_elapsed = time;
    while (!event_queue.empty())
    {
        const TimerEvent& event_next = event_queue.top();
        if (event_next.time_trigger >= time_elapsed)
        {
            break;
        }
        event_queue.pop();
    }
    Timer_Resume();
}