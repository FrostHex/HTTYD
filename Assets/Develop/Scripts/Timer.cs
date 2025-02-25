using UnityEngine;
using System;
using System.Collections.Generic;

// 附着于物体: MainControl, (由MainControl.cs添加), 提供计时和按时触发事件的功能

public class Timer : MonoBehaviour
{
    private SortedList<float, Action> timer_events = new SortedList<float, Action>();
    private float timer = 0f;
    private float event_key;
    private Action event_action;
    private Action current_state = null;

    public void Timer_ChangeState(int new_state)
    {
        switch(new_state)
        {
            case 0:
                current_state = Timer_Stop;
                break;
            case 1:
                current_state = Timer_Begin;
                break;
        }
        Debug.Log("Timer state changed to: " + new_state);
    }

    void FixedUpdate()
    {
        current_state?.Invoke(); // 调用当前状态的函数
    }

    private void Timer_Stop()
    {

    }

    private void Timer_Begin()
    {
        if (timer_events.Count > 0 && timer_events.Keys[0] <= timer)
        {
            event_key = timer_events.Keys[0];
            event_action = timer_events[event_key];

            // 触发事件
            event_action.Invoke();
            // 移除已触发的事件
            timer_events.RemoveAt(0);
        }

        timer += Time.fixedDeltaTime;
        // Debug.Log(timer);
    }

    public void Timer_AddTimerEvent(float time, Action callback)
    {
        if (timer_events.ContainsKey(time))
        {
            timer_events[time] += callback;
        }
        else
        {
            timer_events.Add(time, callback);
        }
    }
}
