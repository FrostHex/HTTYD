# How to Train Your Dragon
![Demo Image](Environment/Media/Demo.png)

## Demo Video
Demo Video: [Environment/Media/Demo.mp4](Environment/Media/Demo.mp4) \
Bilibili Link: [https://www.bilibili.com/video/BV1h5gxzNE4X/](https://www.bilibili.com/video/BV1h5gxzNE4X/)

## Development Environment
- Windows 64-bit
- Godot v4.5-stable
- Godot-cpp

## Quick Start
```bash
git clone https://github.com/FrostHex/HTTYD
cd HTTYD
git submodule update --init
git clone -b godot-cpp-compiled --depth 1  https://github.com/FrostHex/HTTYD temp && mv temp/bin  temp/gen ./Addons/godot-cpp/ && rm -rf temp
```

## Build Godot Cpp
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

## Build the New Code
```bash
cd Scripts/build
scons platform=windows use_mingw=yes bits=64 target=release
```

## Clean the Old Code
```bash
cd Scripts/build
scons -c platform=windows use_mingw=yes bits=64 target=release
```

## Build New Godot-cpp
```bash
git submodule foreach git pull origin master
cd Addons/godot-cpp
"D:\Godot\Godot_v4.5-stable_win64.exe" --dump-extension-api # modify the path of godot.exe in the command
scons platform=windows use_mingw=yes custom_api_file="extension_api.json"
```