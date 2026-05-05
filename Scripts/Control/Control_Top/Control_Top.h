#ifndef CONTROL_TOP_H
#define CONTROL_TOP_H

#include <godot_cpp/classes/node.hpp>

namespace godot
{
    class Control_Top : public Node
    {
        public:
            Control_Top() = default;
            ~Control_Top() override = default;
            virtual void _ready() = 0;

        protected:
            void SyncSkyTime(Node *time_of_day);
    };
}

#endif // CONTROL_TOP_H
