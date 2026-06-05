# Changelog
--------------------------------------------------
This project loosely follows [Semantic Versioning](https://semver.org/) for version numbering, with the following guidelines:
- MAJOR version when you make incompatible API changes
- MINOR version when you add **a set of functionalities** in a backwards-compatible manner (rather than any functionality)
- PATCH version when you make backwards-compatible bug fixes, **minor tweaks and minor feature additions** (rather than bug fix only)

## v0.1.0 - 2022-12-24
- Added: Toothless base model with ZBrush.
- Notes: Holiday tail-fin concept; time was limited so scales were not detailed.

## v0.1.1 - 2024-04-12
- Changed: Rebuilt head and wing structure as a foundation for rigging and eye placement.

## v0.1.2 - 2024-04-14
- Added: Rigging and animation workflow attempt.

## v0.2.0 - 2024-04-29
- Changed: Improved facial topology and functional sphere-shaped eyeball layout.

## v0.2.1 - 2024-05-02
- Added: Saddle and tail-fin models.

## v0.2.2 - 2024-05-04
- Changed: Minor topology tweaks before rigging.

## v0.2.3 - 2024-05-05
- Added: New rigging workflow with Blender

## v0.2.4 - 2024-05-07
- Added: Tgt and Ctrl bones for rigs

## v0.2.5 - 2024-05-10
- Added: Initial pupil rig.

## v0.3.0 - 2024-05-13
- Added: Wing and tail control rigs.
- Notes: Functional rig setup.

## v0.3.1 - 2024-05-21
- Added: Initial importation into Unity
- Added: Camera-to-saddle transform tuned to reduce motion sickness.

## v0.3.2 - 2024-05-26
- Added: Unity VR prototype running in-scene.
- Added: Grass shader with swaying effect.

## v0.3.3 - 2024-05-29
- Changed: Flight control tweaks.

## v0.4.0 - 2024-06-03
- Added: Early volumetric cloud effect.
- Notes: Basic environment setup, basic flight experience 

## v0.4.1 - 2024-07-10
- Added: Basic skeletal animation.

## v0.4.2 - 2024-07-16
- Added: Animation state machine prototype.
- Added: Cheat Sheet + small terrain and cloud assets.

## v0.5.0 - 2024-07-24
- Added: Sea stack environment prototype.
- Notes: Finish the environment setup, Test Drive is still incomplete.

## v0.5.1 - 2025-01-06
- Fixed: Asymmetric pupil weight issues.
- Fixed: Twisted Wing during dive in Unity.

## v0.6.0 - 2025-04-28
- Breaking: Engine migrated from Unity to Godot.
- Added: Godot C++ scripting framework.
- Added: External ocean and volumetric cloud resources.
- Changed: Flight attitude logic and Euler angle handling.
- Notes: Non-physical flight model, minimum airspeed clamped to 3 m/s.

## v0.6.1 - 2025-05-14
- Added: Basic flight animation integration.
- Changed: Sea-stack scene migrated into Godot.
- Notes: AnimationTree filter and state transition still have limitations.

## v0.6.2 - 2025-05-27
- Added: VR headset pose drives flight controls.
- Added: Flight control state machine framework.
- Added: Event timers and secondary camera window.
- Notes: Volumetric cloud rendered wrong when looking up in VR.

## v0.7.0 - 2025-05-30
- Added: Full Cheat Sheet operation flow and state machine.
- Changed: Dynamic reparenting of Cheat Sheet nodes.

## v0.8.0 - 2025-07-05
- Added: Save system (Ctrl+C to save / Ctrl+V to load).
- Added: Test Drive crash rewind, rewind indicator
- Changed: Godot upgraded to 4.5 beta2 with breaking change error fixed.
- Changed: Sea-stack scene refactored into reusable nodes.
- Notes: Disable volumetric clouds plugin for now.

## v0.9.0 - 2025-07-17
- Added: Test Drive second-half flow scaffolded.
- Changed: Finish the sections testing of the first-half.

## v1.0.0 - 2025-07-23
- Added: Full Test Drive experience.
- Notes: Known bugs remain; stability not fully guaranteed.

## v1.1.0 - 2025-09-22
- Added: Tutorial and training modes (3 progressive stages).
- Added: Settings menu (language, helper view, debug info).
- Added: Simple achievement system.
- Added: Path guide marker in practice(free-flight) mode.

## v1.1.1 - 2026-04-20
- Added: Sky3D and location simulation for sunrise/sunset.
- Changed: Refactor dragon node to improve reuse.
- Changed: Quest2 / Pimax Crystal VR compatibility tested.

## v1.1.2 - 2026-05-14
- Added: Card UI system 
- Fixed: Multiple Test Drive minor issues.
- Changed: Test Drive color correction tweaks.

## v1.1.3 - 2026-05-29
- Fixed: A conservative way to compile cpp.
- Changed: Environmental visual improvements.

## v1.1.4
- Added: Rune language support.
- 


----------
## TODO
### Bug Fixes & Optimization
- Fixed: Esc menu in VR.

### Try Some New Stuff
- Added: Plasma blast shaders.