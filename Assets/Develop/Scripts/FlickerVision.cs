using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 附着于物体: XR Origin (XR Rig)/Camera Offset/Main Camera/Dark/Hemisphere
// 设置视野明暗(视野前方遮挡物的透明度变化)

public class FlickerVision : MonoBehaviour
{
    private bool  mode_darken = false;
    float[] alpha_array = new float[75]; // 透明度数组
    int index = 0;
    const int darken_step = 6; // 变暗的步长
    const int lighten_step = 1; // 变亮的步长

    void Awake()
    {
        this.enabled = false;

        // 获取游戏对象的Renderer组件
        Renderer renderer = GetComponent<Renderer>();

        for (int i = 0; i < alpha_array.Length; i++)
        {
            alpha_array[i] = Mathf.Pow((i/74.0f), 2);
        }
    }

    public void Start()
    {
        // 打印数组
        // for (int i = 0; i < alpha_array.Length; i++)
        // { 
        //     Debug.Log(i + ": " + alpha_array[i]);
        // }
    }

    public void Vision_Darken()
    {
        index = 0;
        mode_darken = true;
        this.enabled = true;
    }

    public void Vision_Lighten()
    {
        index = alpha_array.Length - 1;
        mode_darken = false;
        this.enabled = true;
    }

    public void FixedUpdate()
    {
        // 设置_Alpha属性
        GetComponent<Renderer>().material.SetFloat("_Alpha", alpha_array[index]);

        if (mode_darken)
        {
            if (index < alpha_array.Length - darken_step)
            {
                index += darken_step;
            }
            else
            {
                GetComponent<Renderer>().material.SetFloat("_Alpha", 1.0f);
                this.enabled = false;
            }
        }
        else
        {
            if (index > lighten_step -1)
            {
                index -= lighten_step;
            }
            else
            {
                GetComponent<Renderer>().material.SetFloat("_Alpha", 0);
                this.enabled = false;
            }
        }
    }
}
