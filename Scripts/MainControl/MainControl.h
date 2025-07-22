#ifndef MAIN_CONTROL_H
#define MAIN_CONTROL_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/video_stream_player.hpp>
#include "DragonAnimator.h"
#include "CheatSheet.h"
#include "GameTimer.h"
#include "SaveManager.h"

namespace godot 
{
    class MainControl : public Node 
    {
        GDCLASS(MainControl, Node);

        public:
            MainControl();
            ~MainControl();
            void _ready() override;
            void _input(const Ref<InputEvent> &event) override;
            void SetValJoystickInput(bool p_val);
            bool GetValJoystickInput() const;
            void SetValSubView(bool p_val);
            bool GetValSubView() const;

        protected:
            static void _bind_methods();

        private:
            void Initialize_TimerList();
            void Start_Timer();
            void TakeRest();
            bool enable_headset = false;
            bool sub_view = true;
            DragonControlTop* dragon_control;
            DragonAnimator *dragon_animator;
            CheatSheet *cheat_sheet;
            GameTimer* timer;
            CameraControl *camera_ctrl;
            SaveManager* save_manager;
            AudioStreamPlayer* audio_player;
            VideoStreamPlayer* video_player;
    };
}

#endif // MAIN_CONTROL_H