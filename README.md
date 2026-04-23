# PrismTSS - The Shader Sandbox

PrismTSS is a desktop OpenGL shader sandbox and editor for experimenting with shaders. It combines a GLFW/GLAD rendering stack with an ImGui-based interface, runtime shader recompilation, and project persistence so you can iterate on shader code without rebuilding your entire app.

## Features

- Live shader editing with hot reloading for registered shader programs
- An inspector to edit shader uniforms, configure objects and materials, import assets, and link shader programs
- A viewport to preview shader output while editing.
- A console to help see and debug errors.
- Project saving & loading with per-project `shaders/`, `assets/`, and `project.json`.
- Camera controls and context-aware keybindings for editor and viewport.
- Asset support for models, textures, materials.
- Example shaders bundled in to help new developers learn GLSL concepts.

## Requirements

- CMake 3.20 or newer
- A C++20-capable compiler
- `clang` and `clang++` if you use the provided `clang-toolchain.cmake`

Platform notes:

- Linux builds link against OpenGL, X11, `pthread`, `dl`, and `gtk+-3.0` for the native file dialog backend. Packages vary per distribution.
- Windows builds use the standard Win32 and OpenGL system libraries. 
- PrismTSS does not currently support mac.

## Usage

Typical workflow:

1. Launch the app.
2. Create or open a project.
3. Edit shader files in the built-in editor.
4. Save the active shader to trigger recompilation.
5. Configure scene data in the inspector.
6. Inspect results in the viewport.

## Default Controls

- `W` / `A` / `S` / `D`: move camera
- `Space`: move camera up
- `LeftCtrl`: move camera down
- `Shift`: fast camera movement
- `F2`: switch control context
- `Ctrl+S`: save active shader file
- `LeftAlt+S`: save project
- `Ctrl+F`: find in editor
- `F5`: fullscreen viewport
- `F12`: screenshot viewport
- `LeftAlt+F4`: quit

These are the current built-in defaults and may be overridden by saved user settings.

## Troubleshooting

### Shader compile failures

If a shader fails to compile or link, check the app logs or console output. The engine reports stage-specific errors for vertex shaders, fragment shaders, and link failures.

### Missing `project.json`

If a project cannot be loaded and you see messages about `projectJSON` not existing, the app will treat that as a missing or incomplete project and may fall back to a default scene or require a save before data is fully persisted.


## Build

Configure with the provided Clang toolchain:

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./clang-toolchain.cmake
```

If you want faster compilation, keep third-party tests disabled explicitly:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=./clang-toolchain.cmake \
  -DASSIMP_BUILD_TESTS=OFF \
  -DGLFW_BUILD_TESTS=OFF
```

Build the release configuration:

```bash
cmake --build build --config Release
```

Build a debug configuration:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=./clang-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build --config Debug
```

The executable is written to `bin/sandbox`.

## Tests

Build the test target with:

```bash
cmake --build build --target sandbox_tests
./bin/sandbox_tests
```

## License

This project is licensed under the MIT License. See `LICENSE`.

Bundled third-party dependencies are covered by their own licenses. See `THIRD_PARTY_NOTICES.md` for a summary of the third-party components redistributed in this repository.