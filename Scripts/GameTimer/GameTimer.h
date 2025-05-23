#ifndef CUSTOM_TIMER_H
#define CUSTOM_TIMER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <vector>
#include <queue>
#include <algorithm>

namespace godot 
{
    // define a structure to store time points and functions to be triggered
    struct TimerEvent 
    {
        float time_trigger; // absolute trigger time (current time + relative delay)
        Callable callback;  // (Godot Callable) function to be triggered
        int id;             // event ID, used for deletion

        // constructor
        TimerEvent(float time, Callable func, int id_event) : time_trigger(time), callback(func), id(id_event) 
        {
        }

        /**
         * @brief overload the comparison operator for the priority queue
         * @param other (pass by reference) the other TimerEvent to compare with
         * @note the priority only depends on the time_trigger value
         * @note because the queue is a min heap, overload operator> rather than operator<
         * @note the smaller the time_trigger, the higher the priority
         */
        bool operator>(const TimerEvent& other) const 
        {
            return time_trigger > other.time_trigger;
        }
    };

    class GameTimer : public Node 
    {
        GDCLASS(GameTimer, Node);

        public:
            GameTimer();
            ~GameTimer();            
            void _ready() override;
            void _physics_process(double delta) override;

            int Timer_AddEvent(float seconds, Callable callback);
            int Timer_AddEventSinceNow(float seconds, Callable callback);
            void Timer_Reset();
            void Timer_Pause();
            void Timer_Resume();
            float Timer_GetTimeElapsed() const;

        protected:
            static void _bind_methods();
        
        private:
            /**
             * @brief priority queue, the elements with highest priority are at the top and come out first 
             * @param TimerEvent the element type in the queue
             * @param std::vector<TimerEvent> the underlying container type
             * @param std::greater<TimerEvent> the comparison function, changes the type of the queue to a min heap
             */
            std::priority_queue<TimerEvent, std::vector<TimerEvent>, std::greater<TimerEvent>> event_queue;
            
            float time_elapsed; // Elapsed time (seconds)
            bool timer_paused; // Whether paused
            int id_event_next; // ID for the next event
    };

}  // namespace godot

#endif // CUSTOM_TIMER_H