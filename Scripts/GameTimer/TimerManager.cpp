#include "TimerManager.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

void TimerManager::_ready() 
{
    timer = memnew(GameTimer);
    add_child(timer);

    if (!Engine::get_singleton()->is_editor_hint()) // only run when the game is running
    {
        // this: current object, "Function1": method name
        timer->Timer_AddEvent(0.0f, Callable(this, "Function1")); 
        timer->Timer_AddEvent(3.0f, Callable(this, "Function2"));
        timer->Timer_AddEvent(2.0f, Callable(this, "Function3"));
        timer->Timer_AddEvent(1.0f, Callable(this, "Function3"));
        timer->Timer_AddEvent(10.0f, Callable(this, "Function3"));
        timer->Timer_AddEvent(5.0f, Callable(this, "Function3"));
        timer->Timer_AddEvent(4.0f, Callable(this, "Function3"));
    }
}

void TimerManager::Function1() 
{
    UtilityFunctions::print("Function1!");
}
    
void TimerManager::Function2() 
{
    UtilityFunctions::print("Function2!");
}
    
void TimerManager::Function3() 
{
    UtilityFunctions::print("Function3!");
}


void TimerManager::_bind_methods() 
{
    // Register methods to Godot
    ClassDB::bind_method(D_METHOD("Function1"), &TimerManager::Function1);
    ClassDB::bind_method(D_METHOD("Function2"), &TimerManager::Function2);
    ClassDB::bind_method(D_METHOD("Function3"), &TimerManager::Function3);
}
