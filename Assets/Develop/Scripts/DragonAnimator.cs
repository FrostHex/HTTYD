using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 附着于物体: Toothless/ToothlessMesh, 被动接收指令并播放动画

public class DragonAnimator : MonoBehaviour
{
    public Animator animator;
    private string current_state = "po_glide";

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Animator_SetState(int layer, string state, float cross_fade_time)
    {
        if (state != current_state)
        {
            animator.CrossFade(state, cross_fade_time, layer); // 0.5s淡入淡出
            animator.speed = 1.0f;
            current_state = state;
            Debug.Log($"Set animation state to '{state}'.");
        }
    }
    public void Animator_SetWeight(int layer, float weight)
    {
        animator.SetLayerWeight(layer, weight);
    }
}
