#include "GameTimer.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;


/**
 * @brief constructor
 */
GameTimer::GameTimer() : time_elapsed(0.0f), timer_paused(false), id_event_next(1) 
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
    Timer_Reset();
    set_physics_process(true); // 初始化时启用物理处理
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
    UtilityFunctions::print("Added timer event ID ", id_event, " for ", seconds, " seconds (will trigger at ", seconds, ")");
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
    UtilityFunctions::print("Added timer event ID ", id_event, " for ", seconds, " seconds (will trigger at ", time_elapsed + seconds, ")");
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

    // UtilityFunctions::print("Timer _physics_process at ", time_elapsed, " seconds");

    time_elapsed += delta; // update timer
    while (!event_queue.empty()) // process all due events
    {
        const TimerEvent& event_next = event_queue.top(); // get the top event from queue (but don't pop it yet)
        if (event_next.time_trigger > time_elapsed) 
        {
            break; // exit the loop if the next event hasn't reached its trigger time
        }
        event_next.callback.call(); // trigger the event
        UtilityFunctions::print("Timer event ID ", event_next.id, " triggered at ", time_elapsed, " seconds");
        event_queue.pop(); // remove the triggered event
    }
    if (event_queue.empty()) // disable physics processing if no events are left
    {
        set_physics_process(false);
    }
}


/**
 * @brief Reset the timer and all events
 */
void GameTimer::Timer_Reset() 
{
    time_elapsed = 0.0f;
    while (!event_queue.empty()) // clear event queue
    {
        event_queue.pop();
    }
    UtilityFunctions::print("Timer Timer_Reset");
    set_physics_process(false);
}


/**
 * @brief pause the timer
 */
void GameTimer::Timer_Pause() 
{
    timer_paused = true;
    UtilityFunctions::print("Timer Timer_Paused at ", time_elapsed, " seconds");
}


/**
 * @brief resume the timer
 */
void GameTimer::Timer_Resume() 
{
    timer_paused = false;
    UtilityFunctions::print("Timer Timer_Resumed at ", time_elapsed, " seconds");
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