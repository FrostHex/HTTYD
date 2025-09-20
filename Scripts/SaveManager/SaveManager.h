#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot 
{
    class SaveManager : public Node 
    {
        GDCLASS(SaveManager, Node);

        public:
            SaveManager();
            ~SaveManager();
            void _ready() override;
            
            void State_Save(const Dictionary& game_data);
            Dictionary State_Load();
            
            void Settings_Save(const Dictionary& settings_data);
            Dictionary Settings_Load();

        protected:
            static void _bind_methods();

        private:
            String get_executable_directory();
    };
}

#endif // SAVE_MANAGER_H