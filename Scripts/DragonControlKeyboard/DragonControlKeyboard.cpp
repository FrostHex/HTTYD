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


/**
 * @brief get input from the joystick
 * @param input_keys array to store the input values, 0 for linear movement, 1 for pitch, 2 for yaw
 * @note the type of the value in the array is float, with the range of -1 to 1
 */
void DragonControlKeyboard::GetInput(float* input_keys) 
{
    input_keys[0] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_W)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_S));
    
    input_keys[1] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_DOWN)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_UP));
    
    input_keys[2] = static_cast<float>(input_singleton->is_key_pressed(Key::KEY_RIGHT)) - 
                    static_cast<float>(input_singleton->is_key_pressed(Key::KEY_LEFT));
}


void DragonControlKeyboard::_bind_methods() 
{
}