#include "SaveManager.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>

using namespace godot;

/**
 * @brief constructor
 */
SaveManager::SaveManager() 
{
}

/**
 * @brief destructor
 */
SaveManager::~SaveManager() 
{
}

/**
 * @brief called when the node and its children are initialized
 */
void SaveManager::_ready() 
{
    // if (Engine::get_singleton()->is_editor_hint()) 
    // {
    //     return;
    // }
}

/**
 * @brief saves the current game state
 */
void SaveManager::State_Save(const Dictionary& game_data)
{
    UtilityFunctions::print("Saving game state...");

    // path validation
    Ref<DirAccess> dir = DirAccess::open("res://"); // get the Godot file access API
    if (!dir.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to access res:// directory.");
        return;
    }
    if (!dir->dir_exists("Saves")) // check if "Saves" directory exists, if not, create it
    {
        Error err = dir->make_dir("Saves");
        if (err != OK) {
            UtilityFunctions::printerr("Failed to create Saves directory.");
            return;
        }
    }
    String save_file_path = "res://Saves/Save.json"; // check if "Save.json" exists inside "Saves", if not, create it
    if (!dir->file_exists("Saves/Save.json")) 
    {
        Ref<FileAccess> file = FileAccess::open(save_file_path, FileAccess::WRITE);
        if (file.is_valid()) 
        {
            file->store_string("{}"); // write empty JSON object
            file->close();
        } 
        else 
        {
            UtilityFunctions::printerr("Failed to create Save.json file.");
            return;
        }
    }

    // backup existing save file
    Ref<FileAccess> file = FileAccess::open(save_file_path, FileAccess::READ);
    if (file.is_valid()) 
    {
        String content = file->get_as_text();
        file->close();
        content = content.strip_edges();
        if (!content.is_empty() && content != "{}") 
        {
            Ref<DirAccess> saves_dir = DirAccess::open("res://Saves/");
            if (!saves_dir.is_valid()) 
            {
                UtilityFunctions::printerr("Failed to access Saves directory.");
                return;
            }
            
            // find the next backup number
            PackedStringArray files = saves_dir->get_files();
            int max_num = 0;
            for (int i = 0; i < files.size(); ++i) 
            {
                String fname = files[i];
                if (fname.begins_with("Save_Backup_") && fname.ends_with(".json")) 
                {
                    int start = String("Save_Backup_").length();
                    int end = fname.length() - String(".json").length();
                    String num_str = fname.substr(start, end - start);
                    int num = num_str.to_int();
                    if (num > max_num) 
                    {
                        max_num = num;
                    }
                }
            }
            int next_num = max_num + 1;
            String backup_name = vformat("Save_Backup_%02d.json", next_num);
            String backup_path = "res://Saves/" + backup_name;
            Ref<FileAccess> backup_file = FileAccess::open(backup_path, FileAccess::WRITE);
            if (backup_file.is_valid()) 
            {
                backup_file->store_string(content);
                backup_file->close();
                UtilityFunctions::print("Backup created: " + backup_name);
            } 
            else 
            {
                UtilityFunctions::printerr("Failed to create backup file: " + backup_name);
            }
        }
    }

    String json_string = JSON::stringify(game_data);
    Ref<FileAccess> save_file = FileAccess::open(save_file_path, FileAccess::WRITE);
    if (save_file.is_valid()) 
    {
        save_file->store_string(json_string);
        save_file->close();
        UtilityFunctions::print("Game state saved successfully.");
    } 
    else 
    {
        UtilityFunctions::printerr("Failed to save game state.");
    }
}


/**
 * @brief loads the saved game state
 */
Dictionary SaveManager::State_Load()
{
    UtilityFunctions::print("Loading game state...");
    String save_file_path = "res://Saves/Save.json";
    Ref<DirAccess> dir = DirAccess::open("res://Saves/");
    if (!dir.is_valid() || !dir->file_exists("Save.json")) 
    {
        UtilityFunctions::printerr("Save file does not exist.");
        return Dictionary();
    }
    Ref<FileAccess> file = FileAccess::open(save_file_path, FileAccess::READ);
    if (!file.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to open save file.");
        return Dictionary();
    }
    String content = file->get_as_text();
    file->close();
    Ref<JSON> json = memnew(JSON);
    Error parse_result = json->parse(content);
    if (parse_result != OK) 
    {
        UtilityFunctions::printerr("Failed to parse save file JSON.");
        return Dictionary();
    }
    Dictionary game_data = json->get_data();
    UtilityFunctions::print("Game state loaded successfully.");
    return game_data;
}


/**
 * @brief bind methods to the Godot engine
 */
void SaveManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("State_Save", "game_data"), &SaveManager::State_Save);
    ClassDB::bind_method(D_METHOD("State_Load"), &SaveManager::State_Load);
}