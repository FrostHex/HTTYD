#include "DragonControlKeyboard.h"

using namespace godot;

/**
 * @brief constructor
 */
DragonControlKeyboard::DragonControlKeyboard() 
{
    input_singleton = Input::get_singleton();
    set_process_input(true);
}

/**
 * @brief destructor
 */
DragonControlKeyboard::~DragonControlKeyboard() 
{
}

void DragonControlKeyboard::GetInput(float* input_keys) 
{
    input_keys[0] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_W)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_S));
    
    input_keys[1] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_DOWN)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_UP));
    
    input_keys[2] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_RIGHT)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_LEFT));

    // String pressed_keys = "Pressed: ";
    // if (input_singleton->is_key_pressed(Key::KEY_UP)) 
    // {
    //     pressed_keys += "[UP] ";
    // }
    // if (input_singleton->is_key_pressed(Key::KEY_DOWN)) 
    // {
    //     pressed_keys += "[DOWN] ";
    // }
    // if (input_singleton->is_key_pressed(Key::KEY_LEFT)) 
    // {
    //     pressed_keys += "[LEFT] ";
    // }
    // if (input_singleton->is_key_pressed(Key::KEY_RIGHT)) 
    // {
    //     pressed_keys += "[RIGHT] ";
    // }
    // if (!pressed_keys.is_empty()) 
    // {
    //     UtilityFunctions::print(pressed_keys);
    // }
}

void DragonControlKeyboard::_bind_methods() 
{
}