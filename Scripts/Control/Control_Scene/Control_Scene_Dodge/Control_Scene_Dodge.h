#ifndef CONTROL_SCENE_DODGE_H
#define CONTROL_SCENE_DODGE_H

#include <godot_cpp/classes/node.hpp>
#include "Control_Top.h"

namespace godot 
{
    class XRController3D; // forward declaration

    class Control_Main;

    class Control_Scene_Dodge : public Control_Top 
    {
        GDCLASS(Control_Scene_Dodge, Node)

        public:
            Control_Scene_Dodge();
            ~Control_Scene_Dodge() override;
            void _ready() override;

        protected:
            static void _bind_methods();

        private:
    };

} // namespace godot

#endif // CONTROL_SCENE_DODGE_H