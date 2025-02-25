using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using UnityEditor.Animations;

public class AnimationClipProcessor : Editor
{
    [MenuItem("Tools/Process Animation Clip Properties")]
    public static void ProcessAnimationClipProperties()
    {
        // RenameAnimationClips(); // 将fbx中的动画复制到Animation文件夹下并重命名
        FilterAnimationClips(); // 删除不需要的属性
        SetAnimationClips(); // 将动画分配给Animator Controller

    }


    private static void FilterAnimationClips()
    {
        // 获取路径中的所有动画剪辑
        string[] guids = AssetDatabase.FindAssets("t:AnimationClip", new[] { "Assets/Develop/Toothless/Animation" });
        List<AnimationClip> animationClips = new List<AnimationClip>();
        foreach (string guid in guids)
        {
            string path = AssetDatabase.GUIDToAssetPath(guid);
            AnimationClip clip = AssetDatabase.LoadAssetAtPath<AnimationClip>(path);
            if (clip != null)
            {
            animationClips.Add(clip);
            }
        }

        // 处理获取到的动画剪辑
        foreach (var clip in animationClips)
        {
            var bindings = AnimationUtility.GetCurveBindings(clip); // 获取动画剪辑中的所有绑定
            var propertiesToRemove = new List<EditorCurveBinding>(); // 创建一个列表来存储需要删除的属性绑定
            foreach (var binding in bindings)
            {
                // 通用, 删除包含"Tgt"和"Ctrl"的属性
                if (binding.path.Contains("Ctrl") || binding.path.Contains("Tgt"))
                {
                    propertiesToRemove.Add(binding);
                }

                // Eye Layer, 只保留包含"eye"的属性
                if (clip.name.Contains("eye")) 
                {
                    if (!binding.path.Contains("eye"))
                    {
                        propertiesToRemove.Add(binding);
                    }
                }

                // Mouth Layer, 只保留包含"jaw.lower"的属性
                if (clip.name.Contains("mouth")) 
                {
                    if (!binding.path.Contains("jaw.lower"))
                    {
                        propertiesToRemove.Add(binding);
                    }
                }

                // Tail Layer, 只保留包含"tail"的属性
                if (clip.name.Contains("tail")) 
                {
                    if (!binding.path.Contains("tail"))
                    {
                        propertiesToRemove.Add(binding);
                    }
                }


                // // tail_wing, 保留包含"tail.wing"的属性, 删除包含"haunch"或".R"的属性
                // if (!binding.path.Contains("tail.wing") || binding.path.Contains(".R") || binding.path.Contains("haunch"))
                // {
                //     propertiesToRemove.Add(binding);
                // }
                
                // tail_swing, 删除包含"wing"的属性
                // if (binding.path.Contains("wing"))
                // {
                //     propertiesToRemove.Add(binding);
                // }

                // 作为 additive 的 lo_shake 一定要保留所有Deform骨骼的属性

            }




            // 删除不需要的属性
            foreach (var binding in propertiesToRemove)
            {
                AnimationUtility.SetEditorCurve(clip, binding, null);
                Debug.Log($"Removed property '{binding.propertyName}' from clip '{clip.name}'");
            }
        }

        AssetDatabase.SaveAssets();
        Debug.Log("Completed cleaning properties for selected AnimationClips.");
    }




    private static void RenameAnimationClips()
    {
        // 获取Assets/Develop/Toothless/Toothless.fbx中的所有动画剪辑
        string fbxPath = "Assets/Develop/Toothless/Toothless.fbx";
        Object[] assets = AssetDatabase.LoadAllAssetsAtPath(fbxPath);
        List<AnimationClip> animationClips = new List<AnimationClip>();
        foreach (Object asset in assets)
        {
            if (asset is AnimationClip clip)
            {
            animationClips.Add(clip);
            }
        }

        foreach (AnimationClip clip in animationClips)
        {
            if (clip == null) continue; // 增加空引用检查

            // 先将clip的名称存储到局部变量中
            string originalClipName = clip.name;
            string new_name = originalClipName.Replace("rig|", "");
            
            // 创建新的动画剪辑
            AnimationClip newClip = new AnimationClip();
            EditorUtility.CopySerialized(clip, newClip);

            if (new_name.Contains("lo_"))
            {
                SerializedObject serializedClip = new SerializedObject(newClip);
                SerializedProperty clipSettings = serializedClip.FindProperty("m_AnimationClipSettings");
                if (clipSettings != null)
                {
                    SerializedProperty loopTimeProp = clipSettings.FindPropertyRelative("m_LoopTime");
                    if (loopTimeProp != null)
                    {
                        loopTimeProp.boolValue = true;
                        serializedClip.ApplyModifiedProperties();
                    }
                }
            }
            
            // 生成新的动画剪辑路径
            string newClipPath = "Assets/Develop/Toothless/Animation/" + new_name + ".anim";
            AssetDatabase.CreateAsset(newClip, newClipPath);
            Debug.Log($"Copied animation clip '{originalClipName}' to '{newClipPath}'");
        }
        AssetDatabase.SaveAssets();

        // 删除Assets/Develop/Toothless/Animation/路径下所有名称带__preview__的文件
        string[] previewGuids = AssetDatabase.FindAssets("__preview__", new[] { "Assets/Develop/Toothless/Animation" });
        foreach (string guid in previewGuids)
        {
            string path = AssetDatabase.GUIDToAssetPath(guid);
            if (path.Contains("__preview__"))
            {
            AssetDatabase.DeleteAsset(path);
            Debug.Log($"Deleted preview animation clip at '{path}'");
            }
        }
    }


    private static void SetAnimationClips()
    {
        // Path to the folder containing animation clips
        string animationFolder = "Assets/Develop/Toothless/Animation/";
        // Path to the animator controller
        string controllerPath = "Assets/Develop/Toothless/Toothless Animator Controller.controller";

        // Load the Animator Controller
        AnimatorController animatorController = AssetDatabase.LoadAssetAtPath<AnimatorController>(controllerPath);
        if (animatorController == null)
        {
            Debug.LogError($"Animator Controller not found at {controllerPath}");
            return;
        }

        // Iterate through each layer in the Animator Controller
        foreach (var layer in animatorController.layers)
        {
            // Get the state machine of the current layer
            ChildAnimatorState[] states = layer.stateMachine.states;
            foreach (var childState in states)
            {
                string stateName = childState.state.name;
                // Construct the expected path of the animation clip
                string clipPath = animationFolder + stateName + ".anim";
                AnimationClip clip = AssetDatabase.LoadAssetAtPath<AnimationClip>(clipPath);

                if (clip != null)
                {
                    // If found, assign the AnimationClip to the state's motion
                    childState.state.motion = clip;
                    Debug.Log($"Assigned AnimationClip '{clip.name}' to state '{stateName}' in layer '{layer.name}'.");
                }
                else
                {
                    Debug.LogWarning($"AnimationClip for state '{stateName}' not found at path: {clipPath}");
                }
            }
        }

        AssetDatabase.SaveAssets();
        Debug.Log("Completed setting AnimationClips for Animator Controller states.");
    }
}
