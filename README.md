<p align="center"><img src="Media/Demo/Demo0.png" alt="HTTYD Banner" width="800"/></p>
<div align="center">

# How to Train Your Dragon
VR dragon riding experience built with Godot 4 / 基于 Godot 4 的 VR 乘龙飞行体验

[![Demo Video](https://img.shields.io/badge/Demo%20Video-Mp4-8A2BE2.svg)](Media/Demo/Demo.mp4)
[![演示视频](https://img.shields.io/badge/%E6%BC%94%E7%A4%BA%E8%A7%86%E9%A2%91-bilibili-00a8df?logo=bilibili&logoColor=00a8df&color=00a8df)](https://www.bilibili.com/video/BV1h5gxzNE4X/)
![License](https://img.shields.io/badge/License-MIT-green.svg)
[![CodeFactor](https://img.shields.io/codefactor/grade/github/frosthex/httyd?label=CodeFactor)](https://www.codefactor.io/repository/github/frosthex/httyd/overview/main)
![Platform](https://img.shields.io/badge/Platform-Windows-0078d4.svg)

# Table of Contents 目录
</div>

- [Snapshots 游玩截图](#snapshots)
- [Project Introduction 项目介绍](#project-introduction)
- [Playing Recommendation 游玩建议](#playing-recommendation)
- [Development Guide 开发指南](#development-guide)
- [License & Third-Party Assets 许可和第三方资源](#license--third-party-assets)

<div align="center">

## Snapshots 游玩截图 <a id="snapshots"></a>
<p align="center">
  <img src="Media/Demo/Demo1.png" alt="Demo Image 0" width="48%"/>
  <img src="Media/Demo/Demo2.png" alt="Demo Image 1" width="48%"/>
</p>
<p align="center">
  <img src="Media/Demo/Demo3.png" alt="Demo Image 2" width="48%"/>
  <img src="Media/Demo/Demo4.png" alt="Demo Image 3" width="48%"/>
</p>
<p align="center">
  <img src="Media/Demo/Demo5.png" alt="Demo Image 4" width="48%"/>
  <img src="Media/Demo/Demo6.png" alt="Demo Image 5" width="48%"/>
</p>

## Project Introduction 项目介绍 <a id="project-introduction"></a>
The player is Hiccup, the protagonist of the movie How to Train Your Dragon, and the story takes place a few days before he and his dragon partner Toothless test fly. Players need to memorize the control methods on their flight notes, familiarize themselves with the flight route, and do their best during the test flight, while also dealing with unexpected changes such as high-altitude stalling and falling, strong winds blowing the cheat sheet away that players need to catch in mid-air with their controllers, unexpected fog obscuring vision, etc. The VR device enhances the immersion and sense of presence for this journey, and all OpenXR-compatible VR headsets can play it normally. The game also has both Chinese and English languages. Due to poor iteration, the game play and the controls are far less than ideal for now, but there is a mid-term save mechanism that can increase fault tolerance. For the upper limit of operation, a small achievement system is set up: complete Test Drive to get a level 1 silver badge; remain calm in the face of changes and still complete Test Drive according to the predetermined route to get a level 2 gold badge; complete Test Drive according to the predetermined route without crashing or loading at all to get a level 3 purple badge.

玩家就是驯龙高手这部电影的主角Hiccup，故事发生在他和他的龙伙伴Toothless测试飞行的几天前。玩家需要熟记自己飞行笔记上的控制方法，熟悉飞行路线，最终在测试飞行的时候尽力发挥，还要应对突如其来的变数，例如高空失速下坠、强风将考试小抄吹落需要玩家用手柄凌空抓住、意料之外的浓雾遮蔽视野等等。VR设备为这趟旅程增强了沉浸感和代入感，支持OpenXR的VR头显都可以正常游玩，游戏内也设置了中英双语。由于迭代欠佳操纵手感目前可能并不如人意，但其中也设置了中途存档机制可以增加容错。对于操作上限，设置了小规模的成就系统，完成Test Drive以获得一级的银色徽章；临危不乱在变数中仍按预定路线完成Test Drive以获得二级金色徽章；全程没有撞崖失误或读档，一口气按预定路线完成Test Drive以获得三级的紫色徽章。

## Playing Recommendation 游玩建议 <a id="playing-recommendation"></a>
</div>

- **OpenXR**: Any environment that supports OpenXR should be able to run this project (such as Steam VR, Meta Quest Link, etc.). I tested it on Windows only.  
- **Vulkan**: Vulkan needs to be supported by the graphics card and driver.  
- **VR Mode**: VR mode is recommended for better immersion and experience, while the non-VR mode is not fully developed and may have many issues.  
- **Language**: Currently only supports English and Chinese, and the language can be switched in the settings menu.  
- **OpenXR**: 任何支持OpenXR的环境都应该能够运行这个项目（例如Steam VR、Meta Quest Link等）。我只在Windows上测试过。
- **Vulkan**: 需要显卡和驱动支持Vulkan。
- **VR 模式**: 推荐VR模式以获得更好的沉浸感和体验，非VR模式尚未开发完善，可能存在很多问题。
- **语言**: 目前仅支持英语和中文，语言可以在设置菜单中切换。
<div align="center">

## Development Guide 开发指南 <a id="development-guide"></a>
</div>

### Development Environment 开发环境
- Windows 64-bit
- Godot v4.6.1-rc1
- Godot-cpp

### Quick Start for Development 快速开始
```bash
git clone https://github.com/FrostHex/HTTYD
cd HTTYD
git submodule update --init
git clone -b godot-cpp-compiled --depth 1  https://github.com/FrostHex/HTTYD temp && mv temp/bin  temp/gen ./Addons/godot-cpp/ && rm -rf temp
```

### Documentation 文档
[Overall project structure](Media/Docs/Documentation.md)\
[Changelog](Media/Docs/Changelog.md)

### Build Godot Cpp
Reference: [Godot Cpp Docs](https://docs.godotengine.org/en/latest/tutorials/scripting/cpp/gdextension_cpp_example.html) \
I used mingw64 to build Godot Cpp, my build steps are as follows:
- Install MSYS2
- Open Mingw64 terminal by clicking the mingw64.exe
    ```bash
    # for higher efficiency, I used these commands in mingw64 terminal
    cd ./~
    touch .bashrc
    nano .bashrc
    # I added the following lines to .bashrc
    export http_proxy=http://127.0.0.1:7890
    export https_proxy=http://127.0.0.1:7890
    cd /e/Projects/HTTYD/Scripts/build
    # then save and exit file editor by pressing Ctrl+X
    source .bashrc
    ```
    ```bash
    pacman -Syu
    # close the terminal and reopen it
    pacman -S mingw-w64-x86_64-toolchain
    scons -v # to check if scons is installed
    cd /e/Projects/HTTYD/Addons/godot-cpp
    "D:\Godot_History\Godot_v4.5-dev2\Godot_v4.5-dev2_mono_win64.exe" --dump-extension-api # modify the path of godot.exe in the command
    scons platform=windows use_mingw=yes custom_api_file="extension_api.json"
    ```

### Build the New Code
```bash
cd Scripts/build
scons platform=windows use_mingw=yes bits=64 target=release
```

### Clean the Old Code
```bash
cd Scripts/build
scons -c platform=windows use_mingw=yes bits=64 target=release
```

### Build New Godot-cpp
```bash
git submodule foreach git pull origin master
# use Mingw64 terminal for the following commands
cd /e/Projects/HTTYD/Addons/godot-cpp # modify the path of godot-cpp in the command
"D:\Godot\Godot_v4.7-beta1_win64.exe" --dump-extension-api # modify the path of godot.exe in the command
scons platform=windows use_mingw=yes custom_api_file="extension_api.json"
```

### Update the Plugins
Plugins others than Godot-cpp are not imported as submodules, so they need to be copied and overwritten manually while keeping the structure unchanged. Then run `Scripts/Utils/SuitPlugins.py` to substitute the paths, which suits the plugins to the project structure.

### Verbose Running for Debugging
```bash
"D:\Godot\Godot_v4.6.1-rc1_win64.exe" --path "E:\Projects\HTTYD" --verbose # modify the path
```

### Running in VR Mode
- **Quest 2 with default OpenXR**: \
  in project.godot, set `openxr/target_api_version="1.1.53"` under `[xr]` section
- **Pimax with SteamVR**: \
  in project.godot, remove or leave `openxr/target_api_version` empty under `[xr]` section

### Recommended VR Settings for Oculus Debug Tool
**Remember to click `Service -> Restart Oculus Service` after changing the settings!** \
<img src="Media/Demo/OculusDebugToolSettings.png" alt="Oculus Debug Tool Settings" width="35%"/>

<div align="center">

## License & Third-Party Assets 许可和第三方资源 <a id="license--third-party-assets"></a>
</div>

All content in this repository that is not mentioned in this section is covered by the **MIT** License.

Third-party assets for development used in this project are as follows:
- `./Addons/godot-cpp/` [Godot Cpp](https://github.com/godotengine/godot-cpp)
- `./Addons/godot-xr-tools/` [Godot XR Tools](https://github.com/GodotVR/godot-xr-tools)
- `./Addons/sky_3d/` [Skybox](https://github.com/TokisanGames/Sky3D)
- `./Addons/SunshineClouds2/` [Volumetric Clouds](https://github.com/Bonkahe/SunshineClouds2)
- `./Dragons/Gronckle/` [Gronckle Model](https://models.spriters-resource.com/pc_computer/schoolofdragons/asset/330251/)
- `./Image/paper.png` [Parchment Image](https://huaban.com/pins/4028637372/)
- `./Ocean/` [Ocean Surface](https://github.com/2Retr0/GodotOceanWaves)
- `./Rocks/` [Rock Models & Textures](https://github.com/Unity-Technologies/WaterScenes)
- `./Scripts/Utils/sunset-1.1.7/` [Sunset & Sunrise Time Calculation](https://github.com/buelowp/sunset)
- `./Trees/` [Trees](https://www.fab.com/listings/295bd6bb-7955-40c7-b344-17e289db73ea)

Music and video clips used in this project are as follows:
- Test Drive clip and soundtrack from the movie How to Train Your Dragon
- Pathfinder - Alexander Nakarada