using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 附着于物体: Sun
// 设置太阳属性, 包括明暗和旋转

public class FlickerSun : MonoBehaviour
{
    private Light sun; // 太阳光源
    private bool  mode_darken = false;
    private float[] intensity_array = new float[150]; // 亮度数组
    private int index = 0;
    private bool wait_rendering = false; // 是否等待渲染的标志, 用于控制FixedUpdate的频率
    public int target_intensity = 130000;
    private const int lighten_step = 2; // 亮度增加的步长

    void Awake()
    {
        sun = GetComponent<Light>();
        this.enabled = false;

        for (int i = 0; i < intensity_array.Length; i++)
        {
            intensity_array[i] = Mathf.Pow(0.039762f * (i+30), 6);
        }
    }

    public void Start()
    {
        // 打印数组
        // for (int i = 0; i < intensity_array.Length; i++)
        // { 
        //     Debug.Log(i + ": " + intensity_array[i]);
        // }
    }

    public void Sun_SetRotation(float x, float y, float z)
    {
        this.transform.rotation = Quaternion.Euler(x, y, z);
    }
    
    public void Sun_Darken()
    {
        index = intensity_array.Length - 1;
        mode_darken = true;
        this.enabled = true;
    }

    public void Sun_Lighten(int target_intensity)
    {
        this.target_intensity = target_intensity;
        index = 0;
        mode_darken = false;
        this.enabled = true;
    }

    public void FixedUpdate()
    {
        // Debug.Log(index + ": " + intensity_array[index] * target_intensity / 130000);
        sun.intensity = intensity_array[index] * target_intensity / 130000;
        if (mode_darken)
        {
            if (index > 9)
            {
                index-= 10;
            }
            else
            {
                sun.intensity = 0;
                this.enabled = false;
            }
        }
        else
        {
            // 每两次FixedUpdate之间等待一次
            // wait_rendering = !wait_rendering; 
            // if (wait_rendering)
            // {
            //     return;
            // }

            if (index < intensity_array.Length - lighten_step)
            {
                index += lighten_step;
            }
            else
            {
                sun.intensity = target_intensity;
                this.enabled = false;
            }
        }
    }
}
