using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.Rendering;
using UnityEngine.Rendering.HighDefinition;

// 附着于物体: MainControl, 总控制

public class MainControl : CommonOperation
{
    private GameObject hand_r;
    private GameObject hand_l;
    private GameObject toothless;
    private GameObject main_camera;
    private GameObject camera_offset;
    private GameObject cheat_sheet;
    private GameObject cheat_sheet_copy;
    private AudioSource audio_source;
    private AudioSource audio_source_test;
    private Button button_cliff;
    private Button button_test_drive;
    private CheatSheet cheat_sheet_script;
    private CameraTop camera_top;
    private FlickerVision flicker_vision;
    private FlickerSun flicker_sun;
    private Timer timer;
    private Volume sky_fog_volume;
    public VolumeProfile test_drive_volume_profile;
    public bool joystick_input = true;
    public bool enable_camera_stabilization = true;
    public Vector3 camera_offset_input = new Vector3(0, 0, 0);
    private DragonControlTop dragon_control_any; // 多态, DragonControlJoystick 或 DragonControlKeyboard

    void Awake()
    {
        DontDestroyOnLoad(this);

        FindObject<GameObject>("Toothless", out toothless);
        FindObject<GameObject>("Right Hand Model", out hand_r);
        FindObject<GameObject>("Left Hand Model", out hand_l);
        FindObject<GameObject>("Main Camera", out main_camera);
        FindObject<GameObject>("Camera Offset", out camera_offset);
        FindObject<GameObject>("cheat_sheet", out cheat_sheet);
        FindObject<GameObject>("cheat_sheet_copy", out cheat_sheet_copy);
        FindObject<AudioSource>("MainControl", out audio_source);
        FindObject<AudioSource>("UI", out audio_source_test);
        FindObject<CheatSheet>("cheat_sheet_copy", out cheat_sheet_script);
        FindObject<CameraTop>("XR Origin (XR Rig)", out camera_top);
        FindObject<Button>("Button Cliff", out button_cliff);
        FindObject<Button>("Button Test Drive", out button_test_drive);
        FindObject<FlickerVision>("Hemisphere", out flicker_vision);
        FindObject<FlickerSun>("Sun", out flicker_sun);
        FindObject<Volume>("Sky and Fog Volume", out sky_fog_volume);
        timer = gameObject.AddComponent<Timer>();

        if (joystick_input)
        {
            dragon_control_any = toothless.AddComponent<DragonControlJoystick>();
        }
        else // keyboard input
        {
            dragon_control_any = toothless.AddComponent<DragonControlKeyboard>();
        }

        button_cliff.onClick.AddListener(() => Main_ChangeSceneStart("Cliff")); // 按下按钮时切换至Cliff场景
        button_test_drive.onClick.AddListener(() => Main_ChangeSceneStart("TestDrive")); // 按下按钮时切换至TestDrive场景

        camera_top.Camera_SetStabilization(enable_camera_stabilization); // 设置是否启用相机旋转稳定
    
        camera_offset.transform.rotation = Quaternion.Euler(camera_offset_input);

        // cheat_sheet.SetActive(false);
    }

    void Start()
    {
        // timer.Timer_AddTimerEvent(2.99f, audio_source_test.Play); // flap
        // timer.Timer_AddTimerEvent(9, audio_source_test.Play); // flap
        // timer.Timer_AddTimerEvent(80, Main_Falling);

        dragon_control_any.linear_velocity_input = 200.0f;
    }

    private void Main_ChangeSceneStart(string scene_name)
    {
        StartCoroutine(Main_ChangeScene(scene_name)); 
    }

    IEnumerator Main_ChangeScene(string scene_name)
    {
        flicker_vision.Vision_Darken();
        flicker_sun.Sun_Darken();
        yield return new WaitForSeconds(3);
        SceneManager.LoadScene(scene_name);
        flicker_vision.Vision_Lighten();
        if (scene_name == "Cliff" || scene_name == "TestDrive")
        {
            camera_top.Camera_SetParent(toothless);
            if (scene_name == "Cliff")
            {
                flicker_sun.Sun_SetRotation(145, -75, 0);
                flicker_sun.Sun_Lighten(130000);
            }
            else if (scene_name == "TestDrive")
            {
                // flicker_sun.Sun_SetRotation(120, 210, 0);
                // flicker_sun.Sun_SetRotation(90, 210, 0);
                flicker_sun.Sun_SetRotation(110, 30, 0);
                flicker_sun.Sun_Lighten(200000);
                // audio_source.Play();
                timer.Timer_ChangeState(1);
                sky_fog_volume.profile = test_drive_volume_profile;

                // Main_SetFogAttenuation(-1);
                // Main_SetFogAttenuation(20);
                Main_SetFogAttenuation(200);
            }
        }
        // if (scene_name == "Coast")
        // {
        //     camera_top.Camera_SetParent(null);
        // }
    }

    private void Main_Falling()
    {
        cheat_sheet_copy.SetActive(true);
        cheat_sheet.SetActive(false);
        cheat_sheet_script.CheatSheet_Detach(dragon_control_any.dragon_rb.velocity);
        dragon_control_any.Dragon_ChangeState(DragonState.FALLING);
    }

    private void Main_SetFogAttenuation(float value)
    {
        if (sky_fog_volume.profile.TryGet<Fog>(out var fog))
        {
            if (value > 0)
            {
                fog.active = true;
                fog.meanFreePath.value = value;
                // Debug.Log("Fog Attenuation已设置为" + value);
            }
            else
            {
                // 取消雾效
                fog.active = false;
                // Debug.Log("Fog Attenuation已关闭");
            }
        }
        else
        {
            Debug.LogError("Volume Profile中没有找到Fog组件");
        }
    }
}
