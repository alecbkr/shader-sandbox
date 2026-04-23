# Third-Party Notices

This repository redistributes third-party source code and headers. The project
itself is licensed under the MIT License in `LICENSE`, but bundled third-party
components remain under their own license terms.

## Bundled Components

### Dear ImGui

- Path: `lib/imgui`
- License: MIT
- Local license file: `lib/imgui/LICENSE.txt`

### GLFW

- Path: `lib/glfw`
- License: zlib/libpng-style license
- Local license file: `lib/glfw/LICENSE.md`

### Assimp

- Path: `lib/assimp`
- License: BSD-3-Clause-style license, plus additional bundled notices inside the same file
- Local license file: `lib/assimp/LICENSE`

### Native File Dialog Extended (NFD)

- Paths: `lib/nfd`, `include/nfd`
- License: zlib
- License notice location: source and header file notices such as `lib/nfd/nfd_gtk.cpp`, `lib/nfd/nfd_win.cpp`, `lib/nfd/nfd_linux_shared.hpp`, and `include/nfd/nfd.hpp`

### GLM

- Path: `include/glm`
- License: MIT or Happy Bunny
- License notice location: `include/glm/copying.txt`

### stb_image / stb

- Paths: `lib/stb`, `include/stb`
- License: dual-licensed under MIT or Public Domain (Unlicense-style option)
- License notice location: `include/stb/stb_image.h`

### Catch2

- Path: `include/catch2`
- License: Boost Software License 1.0
- License notice location: `include/catch2/catch_amalgamated.cpp`

### nlohmann/json

- Path: `include/nlohmann`
- License: MIT
- License notice location: `include/nlohmann/json.hpp`

### nlohmann/json upstream source tree

- Path: `lib/json-develop`
- License: MIT
- Local license files: `lib/json-develop/LICENSE.MIT` and `lib/json-develop/LICENSES/`

### glad

- Paths: `lib/glad`, `include/glad`
- Generated code license: the generated glad code is described upstream as any of Public Domain, WTFPL, or CC0
- Khronos note: some Khronos specifications are licensed under Apache License 2.0, which may affect generated code derived from those specifications
- Documentation note: upstream glad licensing guidance also references a `LICENSE` file for the source code and various Khronos files

### Assimp bundled dependencies

- Path: `lib/assimp/contrib`
- License: MIT
- Status: Assimp vendors additional third-party code in its contrib tree.

## Distribution Guidance

If you redistribute this project:

- keep the root `LICENSE` file for PrismTSS
- preserve the original license files and header notices for bundled dependencies
- include this file, or an equivalent notices bundle, with source and packaged distributions
- add any missing upstream license texts for vendored code that does not already include a standalone license file in the repository, especially when upstream documentation references them directly
