#ifndef TIMER_EXAMPLE_H
#define TIMER_EXAMPLE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/callable.hpp>
#include "GameTimer.h"

namespace godot 
{
    class TimerManager : public Node 
    {
        GDCLASS(TimerManager, Node);

        public:
            void _ready() override;
            
            void Function1();
            void Function2();
            void Function3();

        protected:
            static void _bind_methods();
            
        private:
            GameTimer* timer;
    };
}  // namespace godot

#endif // TIMER_EXAMPLE_H
