using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

// 不附着于物体, 是底层 DragonControlJoystick.cs 和 DragonControlKeyboard.cs 的父类, 
// 为子类提供了公共模板, 并在上层控制运动

public abstract class DragonControlTop : CommonOperation
{
    protected const float dragon_factor_pitch = 1.0f; // 俯仰角速度系数
    protected const float dragon_factor_roll = 1.0f; // 滚转角速度系数
    protected const float dragon_factor_yaw = 0.03f; // 滚转-偏航速度系数
    protected const int dragon_control_damping = 20; // 改变姿态的阻尼
    protected const int dragon_pitch_max = 50; // 俯仰角最大值
    protected const int dragon_roll_max = 35; // 滚转角最大值
    protected const int dragon_pitch_thresh = 20; // 开始减速的俯仰角阈值
    protected const int dragon_roll_thresh = 10; // 开始减速的滚转角阈值
    public bool joystick_input = true; // 是否使用手柄输入
    protected float dragon_pitch_buildup = 0; // 俯仰速度的累积值
    protected float dragon_roll_buildup = 0; // 滚转速度的累积值
    protected float dragon_pitch_normalized = 0.0f; // 从角色身上读到并标准化后的俯仰角
    protected float dragon_roll_normalized = 0.0f; // 从角色身上读到并标准化后的滚转角
    protected Vector3 angular_velocity = Vector3.zero; // 角速度中间变量
    protected float linear_velocity = 0.0f; // 线速度中间变量
    public float linear_velocity_input = 10.0f; // 额外的线速度, 与重力势能转换出的动能无关
    protected float height_init, height_delta; // 初始高度, 高度差值
    public Rigidbody dragon_rb; // 角色的Rigidbody组件
    protected HandInputRight hand_r; // 创建一个HandInputRight类型的对象
    protected HandInputLeft hand_l; // 创建一个HandInputLeft类型的对象
    protected DragonAnimator dragon_animator; // 创建一个DragonAnimator类型的对象
    protected int BaseLayer, EarLayer, EyeLayer, MouthLayer, TailLayer, VibrationLayer; // 动画层级
    private Action current_state;
    protected abstract void Dragon_SpeenControlAngularAnimated(); // 控制角速度并设置动画
    protected abstract void Dragon_SpeenControlAngular(); // 控制角速度
    protected abstract void Dragon_SpeenControlLinear(); // 控制线速度

    protected void Awake()
    {
        Dragon_ChangeState(DragonState.TEST_ANIMATED); // 初始化状态为测试状态
        
        FindObject<DragonAnimator>("ToothlessMesh", out dragon_animator);

        // 获取动画层级
        BaseLayer = dragon_animator.animator.GetLayerIndex("Base Layer");
        EarLayer = dragon_animator.animator.GetLayerIndex("Ear Layer");
        EyeLayer = dragon_animator.animator.GetLayerIndex("Eye Layer");
        MouthLayer = dragon_animator.animator.GetLayerIndex("Mouth Layer");
        TailLayer = dragon_animator.animator.GetLayerIndex("Tail Layer");
        VibrationLayer = dragon_animator.animator.GetLayerIndex("Vibration Layer");
        
        dragon_rb = GetComponent<Rigidbody>(); // 获取角色的Rigidbody组件
        height_init = transform.position.y; // 获取初始高度

        // 设置动画层级的权重
        // dragon_animator.Animator_SetWeight(MouthLayer, 0);
    }   

    protected void FixedUpdate()
    {

        // 标准化角色的欧拉角
        dragon_pitch_normalized = Dragon_NormalizeEuler(transform.rotation.eulerAngles.x);
        dragon_roll_normalized = Dragon_NormalizeEuler(transform.rotation.eulerAngles.z);

        current_state?.Invoke(); // 调用当前状态的函数
    }
    
    public void Dragon_ChangeState(DragonState new_state)
    {
        switch(new_state)
        {
            case DragonState.TEST:
                current_state = Dragon_Test;
                break;
            case DragonState.TEST_ANIMATED:
                current_state = Dragon_Test_Animated;
                break;
            case DragonState.DEFAULT_ANIMATED:
                current_state = Dragon_Default_Animated;
                break;
            case DragonState.HIT_CLIFF:
                current_state = Dragon_HitCliff;
                break;
            case DragonState.FALLING:
                current_state = Dragon_Falling;
                break;
            case DragonState.CRISIS:
                current_state = Dragon_Crisis;
                break;
            case DragonState.DISABLED:
                current_state = Dragon_Disabled;
                break;
        }
        Debug.Log("Dragon state changed to: " + new_state);
    }

    private void Dragon_Test()
    {
        // 速度控制
        Dragon_SpeenControlAngular();
        Debug.Log("linear velocity: " + linear_velocity + " || pitch: " + dragon_pitch_normalized + " || roll: " + dragon_roll_normalized + " || pitch buildup: " + dragon_pitch_buildup + " || roll buildup: " + dragon_roll_buildup);
    }

    private void Dragon_Test_Animated()
    {
        // 速度控制
        Dragon_SpeenControlAngularAnimated();
        Debug.Log("linear velocity: " + linear_velocity + " || pitch: " + dragon_pitch_normalized + " || roll: " + dragon_roll_normalized + " || pitch buildup: " + dragon_pitch_buildup + " || roll buildup: " + dragon_roll_buildup);
    }

    private void Dragon_Default_Animated()
    {
        // 速度控制
        Dragon_SpeenControlAngularAnimated();
        Dragon_SpeenControlLinear();
        Debug.Log("linear velocity: " + linear_velocity + " || pitch: " + dragon_pitch_normalized + " || roll: " + dragon_roll_normalized + " || pitch buildup: " + dragon_pitch_buildup + " || roll buildup: " + dragon_roll_buildup);
    }

    private void Dragon_HitCliff()
    {
        // 速度控制
        Dragon_SpeenControlAngular();
        Dragon_SpeenControlLinear();
        // Debug.Log("linear velocity: " + linear_velocity + " || pitch: " + dragon_pitch_normalized + " || roll: " + dragon_roll_normalized + " || pitch buildup: " + dragon_pitch_buildup + " || roll buildup: " + dragon_roll_buildup);
    }

    private void Dragon_Falling()
    {
        dragon_rb.angularVelocity = Vector3.zero;
        dragon_rb.velocity -= Vector3.up * 9.8f * Time.fixedDeltaTime;
        Debug.Log("Dragon velocity: " + dragon_rb.velocity);
    //    linear_velocity = dragon_rb.velocity
    }

    private void Dragon_Crisis()
    {
        // 速度控制
        Dragon_SpeenControlAngular();
        Dragon_SpeenControlLinear();
        // Debug.Log("linear velocity: " + linear_velocity + " || pitch: " + dragon_pitch_normalized + " || roll: " + dragon_roll_normalized + " || pitch buildup: " + dragon_pitch_buildup + " || roll buildup: " + dragon_roll_buildup);
    }

    private void Dragon_Disabled()
    {
        Debug.Log("Movement disabled!");
    }

    /*
     * @brief 标准化欧拉角
     * @param euler_angle 角色的欧拉角:
     *
     *                    滚转角: transform.rotation.eulerAngles.z
     *                        0 , 360 
     *                      /        \
     *                    90          270   
     *
     *                    俯仰角: transform.rotation.eulerAngles.x
     *                        270 (300)
     *                         |  
     *                        360  
     *                         0  
     *                         |  
     *                         90 (60) 
     *
     * @return 标准化后的欧拉角:
     *
     *                    滚转角:
     *                        0 , 0
     *                      /       \
     *                    90        -90  
     *
     *                    俯仰角:
     *                        -90 (-60)
     *                         |  
     *                         0  
     *                         0  
     *                         |  
     *                         90 (60) 
     */
    protected float Dragon_NormalizeEuler(float euler_angle)
    {
        if (270 < euler_angle && euler_angle < 360)
        {
            return euler_angle - 360;
        }
        else if (0 < euler_angle && euler_angle < 90)
        {
            return euler_angle;
        }
        else 
        {
            return 0;
        }
    }

    /*
     * @brief 减速函数
     * @param angle 标准化的欧拉角
     * @param angle_thresh 开始减速的角度阈值
     * @param angle_max 角度最大值
     * @return 减速系数(不减速(0, 1)完全减速)
     */
    protected float Dragon_ThreshSlowdown(float angle, int angle_thresh, int angle_max)
    {
        if ((angle_thresh > 0  && angle < angle_thresh) || (angle_thresh < 0 && angle > angle_thresh))
        {
            return 1;
        }
        else
        {
            return (angle_thresh - angle) / (angle_max - angle_thresh) + 1;
        }
    }
    
    /*
     * @brief 处理姿态旋转速度的累积值以实现类似惯性的效果
     * @param buildup 旋转速度的累积值 (值越大则加速越快)
     * @param decay_factor 速度累积值的衰减因子
     * @param eular_angle 标准化的欧拉角
     * @param eular_thresh 开始减速的旋转角度阈值
     * @param eular_max 旋转角度的最大值
     * @return 处理后的累积值
     */
    protected float Dragon_ProcessBulidup(float buildup, float decay_factor, float eular_angle, int eular_thresh, int eular_max)
    {
        buildup = buildup > 1 ? buildup - decay_factor : buildup < -1 ? buildup + decay_factor : 0; // 阻尼, 导致累积值衰减
        buildup = Mathf.Clamp(buildup, -dragon_control_damping, dragon_control_damping); // 限制累积值的范围

        // 在接近旋转阈值时限制累积值, 减速
        if (buildup > 0)
        {
            buildup *= 0.5f + 0.5f * Dragon_ThreshSlowdown(eular_angle, eular_thresh, eular_max); 
        }
        else
        {
            buildup *= 0.5f + 0.5f * Dragon_ThreshSlowdown(eular_angle, -eular_thresh, -eular_max);
        }
        return buildup;
    }
}
