using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Assertions;

// 不附着于物体
// 通用操作类, 包含运动状态枚举、查找Game Object或其组件的方法

public class CommonOperation : MonoBehaviour
{
    // 控制无牙运动的状态
    public enum DragonState
    {
        TEST,
        TEST_ANIMATED,
        DEFAULT_ANIMATED,
        HIT_CLIFF,
        FALLING,
        CRISIS,
        DISABLED
    }

    /*  
     * @brief 引用传递查找场景中的 Game Object 或其组件, 并赋值给传入的对象
     * @param T 泛型参数, 代表查找结果的类型
     * @param name 场景中 Game Object 的名称
     * @param obj 用于接收查找结果的变量
     * @note "out 类型 变量": 代表引用传递
     * @note "where T: UnityEngine.Object": 泛型约束, 限制泛型参数 T 必须是 UnityEngine.Object 或其子类
     */
    public void FindObject<T>(string name, out T obj) where T : UnityEngine.Object
    {
        if (typeof(T) == typeof(GameObject)) // 若 T 为 GameObject 类型
        {
            obj = GameObject.Find(name) as T; // as对前者执行安全的类型转换, 试图转换为T类型, 若失败则返回null赋值给obj
        }
        else
        {
            GameObject find_result = GameObject.Find(name); // 根据名称查找场景中的 Game Object
            obj = find_result != null ? find_result.GetComponent<T>() : null; // 若查找结果不为空则获取其中T类型的组件, 否则返回null
        }

        // 如果直接查找失败，尝试查找子物体
        if (obj == null)
        {
            foreach (GameObject rootObj in UnityEngine.SceneManagement.SceneManager.GetActiveScene().GetRootGameObjects())
            {
                Transform child = rootObj.transform.Find(name);
                if (child != null)
                {
                    if (typeof(T) == typeof(GameObject))
                    {
                        obj = child.gameObject as T;
                        break;
                    }
                    else
                    {
                        obj = child.GetComponent<T>();
                        if (obj != null)
                        {
                            break;
                        }
                    }
                }
            }
        }
        Debug.Assert(obj != null, name + " Missing!"); // 断言查找结果不为空

        // obj = default(T); // obj在结束前必须被赋值, 而T类型不一定有null值, 所以不能赋为null而是赋为T类型的默认值
    }
}