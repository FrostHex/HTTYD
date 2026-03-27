# Dev Note

## Project Hierarchy
    ```
    ├── Addons
    ├── DevLog.md
    ├── DevNote.md
    ├── Dragons
    │   ├── Gronckle
    │   │   ├── Gronckle.glb
    │   │   └── Gronckle_Gronckle.png
    │   └── Toothless
    │       ├── CheatSheet.gdshader
    │       ├── Toothless.glb
    │       ├── Toothless_CheatSheet.png
    │       ├── Toothless_Eye.tres
    │       └── Toothless_eye.png
    ├── Environment
    │   ├── Cloud
    │   ├── Fog
    │   ├── Media
    │   │   ├── Demo.mp4
    │   │   ├── Demo.png
    │   │   ├── Pathfinder.wav
    │   │   └── Test Drive.wav
    │   ├── Metal
    │   ├── Mountain
    │   ├── Ocean
    │   ├── PathMarker.glb
    │   ├── Rocks
    │   └── Tree
    ├── Image
    │   ├── badge_1.png
    │   ├── badge_2.png
    │   ├── badge_3.png
    │   ├── icon.png
    │   └── paper.png
    ├── README.md
    ├── Scenes
    │   ├── Dragons
    │   │   ├── Dragon.tscn
    │   │   ├── Dragon_Temp.tscn
    │   │   ├── Gronckle.tscn
    │   │   └── Toothless.tscn
    │   ├── Main.tscn
    │   ├── Mountain.tscn
    │   ├── Ocean.tscn
    │   ├── Rocks
    │   ├── Scene_Dodge.tscn
    │   ├── Scene_Home.tscn
    │   ├── Scene_Practice.tscn
    │   ├── Scene_TD.tscn
    │   ├── Scene_Tutorial.tscn
    │   ├── Tree.tscn
    │   └── XR.tscn
    ├── Scripts
    │   ├── CheatSheet
    │   │   ├── CheatSheet.cpp
    │   │   └── CheatSheet.h
    │   ├── Control_Camera
    │   │   ├── Control_Camera.cpp
    │   │   └── Control_Camera.h
    │   ├── Control_Main
    │   │   ├── Control_Main.cpp
    │   │   └── Control_Main.h
    │   ├── Control_Scene_Home
    │   │   ├── Control_Scene_Home.cpp
    │   │   └── Control_Scene_Home.h
    │   ├── Control_Scene_Practice
    │   │   ├── Control_Scene_Practice.cpp
    │   │   └── Control_Scene_Practice.h
    │   ├── Control_Scene_TD
    │   │   ├── Control_Scene_TD.cpp
    │   │   └── Control_Scene_TD.h
    │   ├── Control_Scene_Tutorial
    │   │   ├── Control_Scene_Tutorial.cpp
    │   │   └── Control_Scene_Tutorial.h
    │   ├── DragonAnimator
    │   │   ├── DragonAnimator.cpp
    │   │   └── DragonAnimator.h
    │   ├── DragonAnimator_Temp
    │   │   ├── DragonAnimator_Temp.cpp
    │   │   └── DragonAnimator_Temp.h
    │   ├── DragonControlJoystick
    │   │   ├── DragonControlJoystick.cpp
    │   │   └── DragonControlJoystick.h
    │   ├── DragonControlKeyboard
    │   │   ├── DragonControlKeyboard.cpp
    │   │   └── DragonControlKeyboard.h
    │   ├── DragonControlTop
    │   │   ├── DragonControlTop.cpp
    │   │   └── DragonControlTop.h
    │   ├── DragonControl_Dodge
    │   │   ├── DragonControl_Dodge.cpp
    │   │   └── DragonControl_Dodge.h
    │   ├── GameTimer
    │   │   ├── GameTimer.cpp
    │   │   └── GameTimer.h
    │   ├── SaveManager
    │   │   ├── SaveManager.cpp
    │   │   └── SaveManager.h
    │   ├── Utils
    │   │   ├── CopyTransform.gd
    │   │   ├── Delete@Node.gd
    │   │   ├── Mp4ToOgv.py
    │   │   ├── Path.gd
    │   │   ├── Rocks_DeleteLod.gd
    │   │   ├── Rocks_Substitution_Basic.gd
    │   │   └── Rocks_Substitution_Pillar.gd
    │   └── build
    ├── Text
    │   ├── Chinese.json
    │   └── English.json
    ├── export_presets.cfg
    └── project.godot
    ```
## 