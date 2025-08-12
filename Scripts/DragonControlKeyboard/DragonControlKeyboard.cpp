#include "DragonControlKeyboard.h"
#include "Control_Camera.h"

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


/**
 * @brief set angular velocity with high mobility in crisis state
 * @param delta time since last frame
 */
void DragonControlKeyboard::SetMotionAngularCrisis(double delta) 
{
    Basis basis = dragon_rb->get_global_transform().basis;
    angular_velocity_buildup += basis.get_column(2) * this->input_keys[1] * DRAGON_FACTOR_PITCH * 8 * delta
                              + basis.get_column(0) * this->input_keys[2] * DRAGON_FACTOR_ROLL * 8 * delta;
    angular_velocity_buildup *= 0.91f * DRAGON_FACTOR_DAMPING;
    float tilt = basis.get_column(2).dot(Vector3(0,1,0)); // local right vector dot global up vector
    tilt = Math::clamp(tilt, -1.0f, 1.0f); // clamp tilt to [-1, 1]
    Vector3 angular_velocity_posture = Vector3(0,1,0) * std::asin(tilt) * DRAGON_FACTOR_YAW * 3 * delta; // std::asin(tilt) is roll angle
    tilt = basis.get_column(1).dot(Vector3(0,1,0)); // local up vector dot global up vector
    if (tilt < 0.0f) 
    {
        Vector3 axis = basis.get_column(2); // local right vector
        axis.y = 0.0f; // project to xz plane
        if (axis.length() > 0.0f) 
        {
            axis = axis.normalized();
        }
        angular_velocity_posture -= axis * std::asin(tilt) * DRAGON_FACTOR_UPSIDE_DOWN;
    }
    dragon_rb->set_angular_velocity(angular_velocity_buildup + angular_velocity_posture);
}


void DragonControlKeyboard::_bind_methods() 
{
}