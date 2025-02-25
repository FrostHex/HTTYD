using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

// 附着于物体: XR Origin (XR Rig)/Camera Offset/Left Controller/Left Hand Model
// 捕获左手柄的输入

public class HandInputLeft : MonoBehaviour
{
    public Vector2 joystick_value;
    public float trigger_value;
    public float grip_value;
    public InputActionProperty action_pinch;
    public InputActionProperty action_grip;
    public InputActionProperty action_joystick;
    public Animator hand_animator;

    // Update is called once per frame
    void FixedUpdate()
    {
        joystick_value = action_joystick.action.ReadValue<Vector2>(); // 读取手柄摇杆的数值, 写入joystick_value
        // Debug.Log("Joystick Value: " + joystick_value);
        trigger_value = action_pinch.action.ReadValue<float>(); // 读取手柄确认按钮的数值, 写入trigger_value
        Debug.Log("Trigger Value: " + trigger_value);
        grip_value = action_grip.action.ReadValue<float>(); // 读取手柄抓握按钮的数值, 写入grip_value
        hand_animator.SetFloat("Trigger", trigger_value);
        hand_animator.SetFloat("Grip", grip_value);
        // Debug.Log("Joystick Value: " + joystick_value);
    }
}
