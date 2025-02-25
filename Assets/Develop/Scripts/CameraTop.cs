using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// 附着于物体: XR Origin (XR Rig)

public class CameraTop : MonoBehaviour
{
    private Quaternion camera_fixed_rotation;

    void Awake()
    {
        camera_fixed_rotation = transform.rotation; // 记录初始时相机的旋转方向
    }

    public void Camera_SetStabilization(bool enable_camera_stabilization)
    {
        this.enabled = enable_camera_stabilization; // 若启用稳定, 则启用Update()的连续调用
        // print("camera stabilization: " + enable_camera_stabilization);
    }

    void Update() 
    { 
        transform.rotation = camera_fixed_rotation; // 设置相机的旋转方向, 使其不随父物体旋转
    }

    public void Camera_SetParent(GameObject parent)
    {
        transform.SetParent(parent.transform);
        transform.localPosition = new Vector3(0, 0.75f, 0.9f);
    }
}
