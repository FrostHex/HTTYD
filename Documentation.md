# Documentation - v1.0.1
--------------------------------------------------
## 1. Project Hierarchy
- `./Addons/` contains the plugins.
- `./Dragons/` contains dragon models and textures.
- `./Environment/` contains environmental assets such as clouds, trees, rocks, etc.
- `./Media/` contains audio, video, images, and text resources.
- `./Scenes/` contains .tscn files for different scenes.
- `./Scripts/` contains mainly C++ scripts for the most part of the game logic.
- `./Scripts/Utils/` contains utility scripts in various languages, mainly for automation during development, with no practical use when running.
- Here is the detailed hierarchy of the project structure:
    ```
    ├── Addons/
    ├── DevLog.md
    ├── Documentation.md
    ├── Dragons/
    │   ├── Gronckle/
    │   │   ├── Gronckle.glb
    │   │   └── Gronckle_Gronckle.png
    │   └── Toothless/
    │       ├── CheatSheet.gdshader
    │       ├── Toothless.glb
    │       ├── Toothless_CheatSheet.png
    │       ├── Toothless_Eye.tres
    │       └── Toothless_eye.png
    ├── Environment/
    │   ├── Cloud/
    │   ├── Fog/
    │   ├── Metal/
    │   ├── Mountain/
    │   ├── Ocean/
    │   ├── PathMarker.glb
    │   ├── Rocks/
    │   └── Tree/
    ├── LICENSE
    ├── Media/
    │   ├── Audio/
    │   │   ├── Pathfinder.wav
    │   │   └── Test Drive.wav
    │   ├── Demo/
    │   ├── Image/
    │   │   ├── badge_1.png
    │   │   ├── badge_2.png
    │   │   ├── badge_3.png
    │   │   ├── icon.png
    │   │   └── paper.png
    │   ├── Text/
    │   │   ├── Chinese.json
    │   │   └── English.json
    │   └── Video/
    │       └── Test Drive.ogv
    ├── README.md
    ├── Scenes/
    │   ├── Dragons/
    │   │   ├── Dragon.tscn
    │   │   ├── Gronckle.tscn
    │   │   ├── GronckleRoot.tscn
    │   │   ├── Toothless.tscn
    │   │   └── ToothlessRoot.tscn
    │   ├── Main.tscn
    │   ├── Mountain.tscn
    │   ├── Ocean.tscn
    │   ├── Rocks/
    │   ├── Scene_Dodge.tscn
    │   ├── Scene_Home.tscn
    │   ├── Scene_Practice.tscn
    │   ├── Scene_TD.tscn
    │   ├── Scene_Tutorial.tscn
    │   ├── Tree.tscn
    │   └── XR.tscn
    ├── Scripts/
    │   ├── CheatSheet/
    │   │   ├── CheatSheet.cpp
    │   │   └── CheatSheet.h
    │   ├── Control/
    │   │   ├── Control_Camera/
    │   │   │   ├── Control_Camera.cpp
    │   │   │   └── Control_Camera.h
    │   │   ├── Control_Main/
    │   │   │   ├── Control_Main.cpp
    │   │   │   └── Control_Main.h
    │   │   └── Control_Scene/
    │   │       ├── Control_Scene_Home/
    │   │       │   ├── Control_Scene_Home.cpp
    │   │       │   └── Control_Scene_Home.h
    │   │       ├── Control_Scene_Practice/
    │   │       │   ├── Control_Scene_Practice.cpp
    │   │       │   └── Control_Scene_Practice.h
    │   │       ├── Control_Scene_TD/
    │   │       │   ├── Control_Scene_TD.cpp
    │   │       │   └── Control_Scene_TD.h
    │   │       └── Control_Scene_Tutorial/
    │   │           ├── Control_Scene_Tutorial.cpp
    │   │           └── Control_Scene_Tutorial.h
    │   ├── Dragon/
    │   │   ├── Dragon_Animator/
    │   │   │   ├── Dragon_Animator.cpp
    │   │   │   └── Dragon_Animator.h
    │   │   ├── Dragon_Pilot_Dodge/
    │   │   │   ├── Dragon_Pilot_Dodge.cpp
    │   │   │   └── Dragon_Pilot_Dodge.h
    │   │   ├── Dragon_Pilot_Joystick/
    │   │   │   ├── Dragon_Pilot_Joystick.cpp
    │   │   │   └── Dragon_Pilot_Joystick.h
    │   │   ├── Dragon_Pilot_Keyboard/
    │   │   │   ├── Dragon_Pilot_Keyboard.cpp
    │   │   │   └── Dragon_Pilot_Keyboard.h
    │   │   └── Dragon_Pilot_Top/
    │   │       ├── Dragon_Pilot_Top.cpp
    │   │       └── Dragon_Pilot_Top.h
    │   ├── GameTimer/
    │   │   ├── GameTimer.cpp
    │   │   └── GameTimer.h
    │   ├── SaveManager/
    │   │   ├── SaveManager.cpp
    │   │   └── SaveManager.h
    │   ├── Utils/
    │   │   ├── CopyTransform.gd
    │   │   ├── Delete@Node.gd
    │   │   ├── Mp4ToOgv.py
    │   │   ├── Path.gd
    │   │   ├── Rocks_DeleteLod.gd
    │   │   ├── Rocks_Substitution_Basic.gd
    │   │   ├── Rocks_Substitution_Pillar.gd
    │   │   ├── SetAnimationPlayer.gd
    │   │   └── SuitPlugins.py
    │   └── build/
    ├── export_presets.cfg
    ├── project.godot
    └── verbose log.txt
    ```


--------------------------------------------------
--------------------------------------------------
## 2. Scene Structure
the representation of nodes: `NodeName (NodeType)` \
the representation of functions(methods): <kbd>FunctionName</kbd>

### 2.1 Main.tscn
```
Main (Node3D)
├── Control_Main (Control_Main)
│   ├── Control_Camera (Control_Camera)
│   └── SaveManager (SaveManager)
├── Camera_Main (Node3D)
│   ├── Camera_Main_NonXR (Camera3D)
│   ├── XR (Node3D)
│   │   └── ...
│   └── XRToolsPickable (RigidBody3D)
│       └── ...
└── Ocean (Node3D)
    └── ...
```
- `Control_Main`, `Control_Camera`, and `SaveManager` are three high-level scripts whose positions in the scene are fixed.
- `Camera_Main` is encapsulated for both VR and non-VR cameras. Ordinary camera manipulation will only be done on `Camera_Main`.
- When the game runs, one of the `Scene_XXX` will always be instantiated under `Main`. For instance, when the game starts, `Scene_Home` will be instantiated, and the structure looks like this:
    ```
    Main (Node3D)
    ├── Control_Main (Control_Main)
    │   └── ...
    ├── Camera_Main (Node3D)
    │   └── ...
    ├── Ocean (Node3D)
    │   └── ...
    └── Scene_Home (Node3D)
        └── ...
    ```
- After specific UI buttons pressed in the `Scene_Home`, which trigger the scene transition using`"SwitchScene"`, the original `Scene_Home` will be freed by `Control_Main`, and one of the new `Scene_XXX` will be instantiated deferred at the end of this frame, at the same position.

### 2.2 Scene_XXX.tscn
- The `Scene_XXX` is the sub scene for each game mode, containing the specific environment and the dragon, such as `Scene_Home`, `Scene_Practice`, `Scene_TD`, `Scene_Tutorial`, and `Scene_Dodge`, etc.
- For now, every scene except `Scene_Home` contains a dragon node. A basic structure of non-home `Scene_XXX` is as follows, and there are some additional nodes for their specific needs.
    ```
    Scene_XXX (Node3D)
    ├── Control_Scene_XXX (Control_Scene_XXX)
    ├── Environment (WorldEnvironment)
    ├── Sun (DirectionalLight3D)
    ├── UI (Control)
    │   └── Button_Back (Button)
    └── Dragon (RigidBody3D)
        └── ...
    ```
- The script node `Control_Scene_XXX` is responsible for the specific logic of each scene.
- When switching to other `Scene_XXX` from `Scene_Home`, the `Camera_Main` will be reparented to the `Socket_Back` node under `Dragon`, via the deferred call of <kbd>AttachCamera</kbd> in `Control_Main`.
- When the back button is pressed, `Control_Scene_XXX` will reparent `Camera_Main` back to `Main`, and defer the repositioning of `Camera_Main`. `Control_Scene_XXX` will also get the reference of `Control_Main` and call its <kbd>SwitchScene</kbd>. The logic in <kbd>SwitchScene</kbd> will free the current `Scene_XXX`, then defer the instantiation of `Scene_Home`.

### 2.3 Dragon.tscn
```
Dragon (RigidBody3D)
├── SpeciesSlot (Node3D)
│   └── XXXRoot (Node3D)
├── SubViewportContainer (SubViewportContainer)
│   └── SubViewport (SubViewport)
│       ├── Camera_Sub (Camera3D)
│       └── ...
└── Dragon_Animator (Dragon_Animator)
```
- The `Dragon` is the encapsulated dragon node for every dragon in the game. Various dragon species' root nodes are located under `SpeciesSlot`.
- When created, the `Dragon_Animator` will defer the call of <kbd>RefreshBindings</kbd>, which will get the animation information from the current dragon species. 
  - In actual running, since `Scene_XXX` that contains `Dragon` is instantiated deferred, the timing of the deferred call of <kbd>RefreshBindings</kbd> is even after the the new scene instantiation
  - When switching to `Scene_Dodge`, the default dragon species is Gronckle rather than Toothless. `Dragon_Pilot_Dodge`, located under `Dragon`, will directly free the `ToothlessRoot` under `SpeciesSlot` and instantiate the `GronckleRoot`. This happens before the deferred call of <kbd>RefreshBindings</kbd>, which ensures the correct animation information obtained from Gronckle rather than Toothless.

### 2.4 DragonRoot.tscn
```
XXXRoot (Node3D)
├── Model (Node3D)
│   └── XXX (Node3D)
│       ├── rig (Node3D)
│       │   └── Skeleton3D (Skeleton3D)
│       │       └── ...
│       └── AnimationPlayer (AnimationPlayer)
├── Sockets (Node3D)
│   └── ...
├── CollisionShapes (Node3D)
│   └── ...
└── AnimationTree (AnimationTree)
```
- Various `XXXRoot` can be set under `SpeciesSlot` in `Dragon`. Various dragon species may have different settings of sockets and collision shapes, while this structure is a common pattern for all of them. For instance, the structure of `ToothlessRoot` is as follows:
    ```
    ToothlessRoot (Node3D)
    ├── Model (Node3D)
    │   └── Toothless (Node3D)
    │       ├── rig (Node3D)
    │       │   └── Skeleton3D (Skeleton3D)
    │       │       └── ...
    │       └── AnimationPlayer (AnimationPlayer)
    ├── Sockets (Node3D)
    │   └── Socket_Back_Mount (BoneAttachment3D)
    │       └── Socket_Back (Marker3D)
    ├── CollisionShapes (Node3D)
    │   ├── CollisionShape_Wing (CollisionShape3D)
    │   └── CollisionShape_Head (CollisionShape3D)
    └── AnimationTree (AnimationTree)
    ```
- After importing the dragon model from, say, `Toothless.glb`, a **new inherited scene** `Toothless` need to be created from this glb.
  - This ensures the hot update of the model file. If the original `Toothless.glb` is modified, there will be no manual operation needed at all. The engine will automatically reimport and show the updated model in the editor.
- The new inherited scene `Toothless` should be placed under `Model` node in `ToothlessRoot`.
- The sockets can be customized for each dragon species, while `Socket_Back` need to present for the camera attachment. The socket mount can be either BoneAttachment3D or normal Node3D, depending on whether the socket needs to follow the animation of a specific bone. For instance, `Socket_Back_Mount` copies the transform of the neck bone of Toothless, so that the camera can follow his neck movement. The transform of the socket can be adjusted, which represents the offset from the target bone.
  - Note that setting the type of socket node to Marker3D is just for better visualization in the editor, and in this project the **red +x axis is the head direction** for everything, and +y for up; +z for right.