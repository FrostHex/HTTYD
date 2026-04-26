#include "SaveManager.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>

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
 * @brief get save base directory for current runtime environment
 */
String SaveManager::get_executable_directory()
{
    if (OS::get_singleton()->has_feature("editor"))
    {
        // In editor/debug runs, store data in the project root (res://).
        return ProjectSettings::get_singleton()->globalize_path("res://").rstrip("/");
    }

    String executable_path = OS::get_singleton()->get_executable_path();
    return executable_path.get_base_dir();
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
    // UtilityFunctions::print("Saving game state...");

    // path validation
    String exe_dir = get_executable_directory();
    String saves_dir_path = exe_dir + "/Saves";
    
    Ref<DirAccess> dir = DirAccess::open(exe_dir); // get the directory access API for executable directory
    if (!dir.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to access executable directory: " + exe_dir);
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
    String save_file_path = saves_dir_path + "/Save.json"; // check if "Save.json" exists inside "Saves", if not, create it
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
            Ref<DirAccess> saves_dir = DirAccess::open(saves_dir_path);
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
            String backup_path = saves_dir_path + "/" + backup_name;
            Ref<FileAccess> backup_file = FileAccess::open(backup_path, FileAccess::WRITE);
            if (backup_file.is_valid()) 
            {
                backup_file->store_string(content);
                backup_file->close();
                // UtilityFunctions::print("Backup created: " + backup_name);
            } 
            else 
            {
                UtilityFunctions::printerr("Failed to create backup file: " + backup_name);
            }
        }
    }

    String json_content = "{\n";
    if (game_data.has("time")) 
    {
        json_content += "\t\"time\": " + String::num(static_cast<float>(game_data["time"])) + ",\n";
    }
    Array keys = game_data.keys();
    for (int i = 0; i < keys.size(); i++) 
    {
        String key = keys[i];
        if (key != "time")
        {
            Variant value = game_data[key];
            json_content += "\t\"" + key + "\": ";
            if (value.get_type() == Variant::STRING) 
            {
                json_content += "\"" + String(value) + "\"";
            }
            else if (value.get_type() == Variant::ARRAY) 
            {
                json_content += JSON::stringify(value);
            }
            else 
            {
                json_content += String(value);
            }
            if (i < keys.size() - 1) 
            {
                json_content += ",";
            }
            json_content += "\n";
        }
    }
    json_content += "}";
    Ref<FileAccess> save_file = FileAccess::open(save_file_path, FileAccess::WRITE);
    if (save_file.is_valid()) 
    {
        save_file->store_string(json_content);
        save_file->close();
        // UtilityFunctions::print("Game state saved successfully.");
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
    String exe_dir = get_executable_directory();
    String saves_dir_path = exe_dir + "/Saves";
    String save_file_path = saves_dir_path + "/Save.json";
    
    Ref<DirAccess> dir = DirAccess::open(saves_dir_path);
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
    // UtilityFunctions::print("Game state loaded successfully.");
    return game_data;
}


/**
 * @brief saves the current settings
 */
void SaveManager::Settings_Save(const Dictionary& settings_data)
{
    UtilityFunctions::print("Saving settings...");

    // path validation
    String exe_dir = get_executable_directory();
    String saves_dir_path = exe_dir + "/Saves";
    
    Ref<DirAccess> dir = DirAccess::open(exe_dir); // get the directory access API for executable directory
    if (!dir.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to access executable directory: " + exe_dir);
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

    String settings_file_path = saves_dir_path + "/Settings.json";
    String json_content = "{\n";
    
    // Add language setting
    if (settings_data.has("language")) 
    {
        json_content += "\t\"language\": " + String::num(static_cast<int>(settings_data["language"])) + ",\n";
    }
    
    // Add enable_headset setting
    if (settings_data.has("enable_headset")) 
    {
        json_content += "\t\"enable_headset\": " + String(static_cast<bool>(settings_data["enable_headset"]) ? "true" : "false") + ",\n";
    }
    
    // Add sub_view setting
    if (settings_data.has("sub_view")) 
    {
        json_content += "\t\"sub_view\": " + String(static_cast<bool>(settings_data["sub_view"]) ? "true" : "false") + ",\n";
    }
    
    // Add debug setting
    if (settings_data.has("debug")) 
    {
        json_content += "\t\"debug\": " + String(static_cast<bool>(settings_data["debug"]) ? "true" : "false") + ",\n";
    }
    
    // Add badge setting
    if (settings_data.has("badge")) 
    {
        json_content += "\t\"badge\": " + String::num(static_cast<int>(settings_data["badge"])) + "\n";
    }
    
    json_content += "}";
    
    Ref<FileAccess> settings_file = FileAccess::open(settings_file_path, FileAccess::WRITE);
    if (settings_file.is_valid()) 
    {
        settings_file->store_string(json_content);
        settings_file->close();
        UtilityFunctions::print("Settings saved successfully.");
    } 
    else 
    {
        UtilityFunctions::printerr("Failed to save settings.");
    }
}


/**
 * @brief loads the saved settings
 */
Dictionary SaveManager::Settings_Load()
{
    // UtilityFunctions::print("Loading settings...");
    String exe_dir = get_executable_directory();
    String saves_dir_path = exe_dir + "/Saves";
    String settings_file_path = saves_dir_path + "/Settings.json";
    
    // Check if settings file exists
    Ref<DirAccess> dir = DirAccess::open(exe_dir);
    if (!dir.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to access executable directory: " + exe_dir);
        return Dictionary();
    }
    
    if (!dir->dir_exists("Saves")) 
    {
        Error err = dir->make_dir("Saves");
        if (err != OK) {
            UtilityFunctions::printerr("Failed to create Saves directory.");
            return Dictionary();
        }
    }
    
    if (!dir->file_exists("Saves/Settings.json")) 
    {
        // Create default settings file
        Dictionary default_settings;
        default_settings["language"] = 0; // English
        default_settings["enable_headset"] = false;
        default_settings["sub_view"] = true;
        default_settings["debug"] = true;
        default_settings["badge"] = 0; // 初始徽章为0（透明）
        
        Settings_Save(default_settings);
        UtilityFunctions::print("Created default settings file.");
        return default_settings;
    }
    
    Ref<FileAccess> file = FileAccess::open(settings_file_path, FileAccess::READ);
    if (!file.is_valid()) 
    {
        UtilityFunctions::printerr("Failed to open settings file.");
        return Dictionary();
    }
    
    String content = file->get_as_text();
    file->close();
    
    Ref<JSON> json = memnew(JSON);
    Error parse_result = json->parse(content);
    if (parse_result != OK) 
    {
        UtilityFunctions::printerr("Failed to parse settings file JSON.");
        return Dictionary();
    }
    
    Dictionary settings_data = json->get_data();
    // UtilityFunctions::print("Settings loaded successfully.");
    return settings_data;
}


/**
 * @brief bind methods to the Godot engine
 */
void SaveManager::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("State_Save", "game_data"), &SaveManager::State_Save);
    ClassDB::bind_method(D_METHOD("State_Load"), &SaveManager::State_Load);
    ClassDB::bind_method(D_METHOD("Settings_Save", "settings_data"), &SaveManager::Settings_Save);
    ClassDB::bind_method(D_METHOD("Settings_Load"), &SaveManager::Settings_Load);
}