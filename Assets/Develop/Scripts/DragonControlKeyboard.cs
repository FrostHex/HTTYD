using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 附着于物体: Toothless, (由MainControl.cs添加)
// 继承 DragonControlTop 类, 实现了键盘控制的 Dragon_SpeenControlAngular() 和 Dragon_SpeenControlLinear() 方法

/*  Y
 *  ^
 *  │     Z (forward)
 *  │    ^
 *  │   /
 *  │  /
 *  │ /
 *  └─────────────> X
 *  左手系
 *  绕x: 俯仰
 *  绕z: 滚转
 */

public class DragonControlKeyboard : DragonControlTop
{
    // 控制线速度
    protected override void Dragon_SpeenControlLinear()
    {
        // 输入控制加速减速
        if (Input.GetKey(KeyCode.W))
        {
            linear_velocity_input += 0.05f; // 控制加速
        }
        else if (Input.GetKey(KeyCode.S))
        {
            linear_velocity_input -= 0.05f; // 控制减速
        }

        // 模拟重力势能与动能的近似转换
        height_delta = height_init - transform.position.y; // 本应是transform.position.y - height_init, 反过来写为了方便下一行少写个负号
        linear_velocity = linear_velocity_input + Mathf.Sign(height_delta) * Mathf.Sqrt(19.6f * Mathf.Abs(height_delta)); // 重力势能转动能
        
        // 模拟空气阻力, 低速下效果甚微, 忽略
        // float f = 0.004f * linear_velocity * linear_velocity / 30;
        // linear_velocity -= f;
        // Debug.Log("f: " + f);

        // linear_velocity = Mathf.Clamp(linear_velocity, 3, 150);
        if (linear_velocity < 3)
        {
            linear_velocity_input += 3 - linear_velocity; // 防止失速, 自动输入额外的能量保持速度为3
            linear_velocity = 3;
        }

        // 将线速度赋给角色
        dragon_rb.velocity = transform.forward * linear_velocity;
    }

    // 控制角速度
    protected override void Dragon_SpeenControlAngular()
    {
        // 打印世界坐标系的角速度
        // Debug.Log("X: " + dragon_rb.angularVelocity.x + " Y: " + dragon_rb.angularVelocity.y + " Z: " + dragon_rb.angularVelocity.z);
        
        // 打印局部坐标系的角速度
        // Vector3 localAngularVelocity = transform.InverseTransformDirection(dragon_rb.angularVelocity);
        // Debug.Log("Local Angular Velocity: X: " + localAngularVelocity.x + " Y: " + localAngularVelocity.y + " Z: " + localAngularVelocity.z);

        // 打印角色的四元数
        // Debug.Log("X: " + transform.rotation.x + " Y: " + transform.rotation.y + " Z: " + transform.rotation.z + " W: " + transform.rotation.w);

        // 打印角色的欧拉角
        // Debug.Log("pitch: " + transform.rotation.eulerAngles.x + " ||yaw: " + transform.rotation.eulerAngles.y + " ||roll: " + transform.rotation.eulerAngles.z);

        // 设置角速度
        angular_velocity = Vector3.zero;
        if (Input.GetKey(KeyCode.UpArrow) && dragon_pitch_normalized < dragon_pitch_max)
        {
            // transform.right: 在世界坐标系下, 指向角色右侧的单位向量
            // Debug.Log("X: " + transform.right.x + " Y: " + transform.right.y + " Z: " + transform.right.z);
            dragon_pitch_buildup += 1.75f;
        }
        else if (Input.GetKey(KeyCode.DownArrow) && dragon_pitch_normalized > -dragon_pitch_max)
        {
            dragon_pitch_buildup -= 1.75f;
        }
        if (Input.GetKey(KeyCode.LeftArrow) && dragon_roll_normalized < dragon_roll_max)
        {
            dragon_roll_buildup += 3;
        }
        else if (Input.GetKey(KeyCode.RightArrow) && dragon_roll_normalized > -dragon_roll_max)
        {
            dragon_roll_buildup -= 3;
        }
        dragon_pitch_buildup = Dragon_ProcessBulidup(dragon_pitch_buildup, 1, dragon_pitch_normalized, dragon_pitch_thresh, dragon_pitch_max);
        angular_velocity += transform.right * dragon_pitch_buildup / dragon_control_damping * dragon_factor_pitch;
        dragon_roll_buildup = Dragon_ProcessBulidup(dragon_roll_buildup, 1.5f, dragon_roll_normalized, dragon_roll_thresh, dragon_roll_max);
        angular_velocity += transform.forward * dragon_roll_buildup / dragon_control_damping * dragon_factor_roll;

        // 将滚转与偏航耦合
        // 左手系中, 正的滚转对应向左偏航, 绕世界坐标系z轴的负方向旋转, 故为-=
        angular_velocity -= dragon_roll_normalized * Vector3.up * dragon_factor_yaw;

        // 将角速度赋给角色
        dragon_rb.angularVelocity = angular_velocity;
    }

    // 控制角速度并设置动画
    protected override void Dragon_SpeenControlAngularAnimated()
    {
        // 设置角速度
        angular_velocity = Vector3.zero;
        if (Input.GetKey(KeyCode.UpArrow) && dragon_pitch_normalized < dragon_pitch_max)
        {
            dragon_pitch_buildup += 1.75f;
            dragon_animator.Animator_SetState(BaseLayer, "po_dive", 0.75f);
        }
        else if (Input.GetKey(KeyCode.DownArrow) && dragon_pitch_normalized > -dragon_pitch_max)
        {
            dragon_pitch_buildup -= 1.75f;
            dragon_animator.Animator_SetState(BaseLayer, "lo_up", 0.75f);
        }
        else if (!Input.GetKey(KeyCode.UpArrow) && !Input.GetKey(KeyCode.DownArrow) && Mathf.Abs(dragon_pitch_normalized) < 20)
        {
            dragon_animator.Animator_SetState(BaseLayer, "po_glide", 0.75f);
        }
        if (Input.GetKey(KeyCode.LeftArrow) && dragon_roll_normalized < dragon_roll_max)
        {
            dragon_roll_buildup += 3;
            dragon_animator.Animator_SetState(BaseLayer, "po_left", 0.75f);
        }
        else if (Input.GetKey(KeyCode.RightArrow) && dragon_roll_normalized > -dragon_roll_max)
        {
            dragon_roll_buildup -= 3;
            dragon_animator.Animator_SetState(BaseLayer, "po_right", 0.75f);
        }
        dragon_pitch_buildup = Dragon_ProcessBulidup(dragon_pitch_buildup, 1, dragon_pitch_normalized, dragon_pitch_thresh, dragon_pitch_max);
        angular_velocity += transform.right * dragon_pitch_buildup / dragon_control_damping * dragon_factor_pitch;
        dragon_roll_buildup = Dragon_ProcessBulidup(dragon_roll_buildup, 1.5f, dragon_roll_normalized, dragon_roll_thresh, dragon_roll_max);
        angular_velocity += transform.forward * dragon_roll_buildup / dragon_control_damping * dragon_factor_roll;

        // 将滚转与偏航耦合
        // 左手系中, 正的滚转对应向左偏航, 绕世界坐标系z轴的负方向旋转, 故为-=
        angular_velocity -= dragon_roll_normalized * Vector3.up * dragon_factor_yaw;

        // 将角速度赋给角色
        dragon_rb.angularVelocity = angular_velocity;
    }
}