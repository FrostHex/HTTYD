# ifndef CONTROL_MAIN_H
# define CONTROL_MAIN_H

#include "Control_Camera.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/viewport.hpp>

namespace godot
{
    enum Language {
        LANGUAGE_ENGLISH = 0,
        LANGUAGE_CHINESE = 1
    };

    class SaveManager; // Forward declaration

    class Control_Main : public Node // Change made here
    {
        GDCLASS(Control_Main, Node); // Change made here

        public:
            Control_Main();
            ~Control_Main();
            void _ready();
            void Switch_Scene(const String &scene_name);
            void SetValEnableHeadset(bool val);
            bool GetValEnableHeadset() const { return enable_headset; } // the const keyword indicates that this function does not modify the instance variables
            void SetValSubView(bool val);
            bool GetValSubView() const { return sub_view; }
            void SetValDebug(bool val);
            bool GetValDebug() const { return debug; }
            void SetValLanguage(int val);
            int GetValLanguage() const { return static_cast<int>(language); }
            void SetValBadge(int val);
            int GetValBadge() const { return badge; }
            void LoadSettings();
            void SaveSettings();

        protected:
            static void _bind_methods();

        private:
            void AttachCamera(const String &scene_name);
            void AttachSunshineClouds(const String &scene_name, bool attach);
            bool enable_headset = false;
            bool sub_view = true;
            bool debug = true; // debug mode
            Language language = LANGUAGE_ENGLISH; // default language
            int badge = 0; // 徽章状态：0-透明，1-3对应不同徽章图片
            Control_Camera* ctrl_camera;
            Node3D* camera_main = nullptr;
            SaveManager* save_manager = nullptr;
            bool is_loading_settings = false; // flag to prevent saving during loading
    };
}

#endif // CONTROL_MAIN_H