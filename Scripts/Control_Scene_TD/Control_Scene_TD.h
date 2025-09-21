#ifndef CONTROL_SCENE_TD_H
#define CONTROL_SCENE_TD_H

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
    class Control_Main; // Forward declaration

    class Control_Scene_TD : public Node 
    {
        GDCLASS(Control_Scene_TD, Node);

        public:
            Control_Scene_TD();
            ~Control_Scene_TD();
            void _ready() override;
            void _input(const Ref<InputEvent> &event) override;

        protected:
            static void _bind_methods();

        private:
            void Initialize_TimerList();
            void Start_Timer();
            void TakeRest();
            void AutoSave(); // 自动存档方法
            // 成就与徽章逻辑
            void _on_td_area_1_body_entered(Node* body);
            void _on_td_area_2_body_entered(Node* body);
            void _update_badge_on_completion();
            bool visited_area_1 = false;
            bool visited_area_2 = false;
            bool used_load_state = false; // 本轮是否使用过读档
            Node* td_area_1 = nullptr;
            Node* td_area_2 = nullptr;
            Control_Main* control_main = nullptr; // reference to Control_Main for accessing shared variables
            DragonControlTop* dragon_control;
            DragonAnimator *dragon_animator;
            CheatSheet *cheat_sheet;
            GameTimer* timer;
            Control_Camera *ctrl_camera;
            SaveManager* save_manager;
            AudioStreamPlayer* audio_player;
            VideoStreamPlayer* video_player;
    };
}

#endif // CONTROL_SCENE_TD_H