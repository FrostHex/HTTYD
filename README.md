# How to Train Your Dragon

## Develpment Environment
- Windows 64-bit
- Godot v4.5-dev2
- Godot-cpp

## Quick Start
```bash
git clone https://github.com/FrostHex/HTTYD
cd HTTYD
git submodule update --init
git clone -b godot-cpp-compiled --depth 1  https://github.com/FrostHex/HTTYD temp && mv temp/bin  temp/gen ./Addons/godot-cpp/ && rm -rf temp
```

## Build New Code
```bash
cd Scripts
scons platform=windows use_mingw=yes bits=64
```

## Build New Godot-cpp
```bash
cd Addons/godot-cpp
git submodule update --init --recursive
# modify the godot.exe path in the command
"D:\Godot\Godot_v4.5-dev2_mono_win64.exe" --dump-extension-api
scons platform=windows use_mingw=yes bits=64 custom_api_file="extension_api.json"
```