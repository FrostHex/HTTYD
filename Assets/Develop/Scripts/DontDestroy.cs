using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 让物体在切换场景时不被销毁

public class DontDestroy : MonoBehaviour
{
    void Awake()
    {
        DontDestroyOnLoad(this);
    }
}
