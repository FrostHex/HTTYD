using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

// 附着于物体: Toothless/ToothlessMesh/cheat_sheet_copy
// 让脱离后的 "cheat sheet 副本" 飘动

public class CheatSheet : MonoBehaviour
{
    protected Rigidbody cheat_sheet_rb;
    private Vector3 initial_detach_velocity = new Vector3(0, -0.33f, -0.2f);
    private Renderer renderer;
    public Material grip_material;

    // Start is called before the first frame update
    void Start()
    {
        cheat_sheet_rb = GetComponent<Rigidbody>();
        renderer = GetComponent<Renderer>();
        XRGrabInteractable interactable = GetComponent<XRGrabInteractable>();
        interactable.onSelectEntered.AddListener(CheatSheet_SetMaterial);

        gameObject.SetActive(false); // 隐藏物体
        this.enabled = false; // 禁用周期函数
    }

    public void CheatSheet_Detach(Vector3 dragon_velocity)
    {
        transform.parent = null; // 解除父子关系
        cheat_sheet_rb.velocity = dragon_velocity + initial_detach_velocity; // 从夹子脱离时的初速度
        this.enabled = true; // 启用周期函数
    }

    public void CheatSheet_SetMaterial(XRBaseInteractor interactor)
    {
        renderer.material = new Material(grip_material);
        renderer.material.SetFloat("_WindDensity", 10);
        renderer.material.SetFloat("_WindStrength", 0.15f);
        renderer.material.SetFloat("_WindOffset", -1.5f);
    }

    void FixedUpdate()
    {
        Debug.Log("CheatSheet velocity: " + cheat_sheet_rb.velocity);
        // Debug.Log("CheatSheet is detached");
        cheat_sheet_rb.velocity -= Vector3.up * Random.Range(7, 12) * Time.fixedDeltaTime;
        cheat_sheet_rb.angularVelocity += new Vector3(Random.Range(-2, 2), Random.Range(-2, 2), Random.Range(-2, 2));
        // cheat_sheet_rb.velocity -= Vector3.up * 9.8f * Time.fixedDeltaTime;
    }
}
