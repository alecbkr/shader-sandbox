cmake_minimum_required(VERSION 3.20)
project(MyProject C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
# Output to ./bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# Generate compile commands for clang LSP to use.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# ---------------------------------------------------------
# ADD GLFW (source version)
# ---------------------------------------------------------
add_subdirectory(lib/glfw)
# ---------------------------------------------------------
# RECURSIVE SOURCE COLLECTION
# ---------------------------------------------------------
file(GLOB_RECURSE SRC_CPP "${CMAKE_SOURCE_DIR}/src/*.cpp" )
set(IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
)
set(STB_SRC ${CMAKE_SOURCE_DIR}/lib/stb/stb_image.cpp )
set(SRC_C "${CMAKE_SOURCE_DIR}/lib/glad/glad.c" )
# ---------------------------------------------------------
# EXECUTABLE
# ---------------------------------------------------------
set(WINDOWS_RESOURCES)

add_executable(sandbox
    ${SRC_CPP}
    ${SRC_C}
    ${IMGUI_SRC}
    ${STB_SRC}
)

# Normal C/C++ include dirs (won't affect RC anymore)
target_include_directories(sandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/stb
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib/glfw/include
)

# ---- Windows RC in its own target (no include dirs passed to windres) ----
if (WIN32)
    add_library(sandbox_rc OBJECT ${CMAKE_SOURCE_DIR}/src/app.rc)
    set_target_properties(sandbox_rc PROPERTIES LINKER_LANGUAGE C)

    # Add the compiled .obj from the rc target into the exe
    target_sources(sandbox PRIVATE $<TARGET_OBJECTS:sandbox_rc>)
endif()
# ---------------------------------------------------------
# LINK GLFW AND SYSTEM LIBS
# (GLFW builds a target called glfw)
# ---------------------------------------------------------
target_link_libraries(sandbox glfw)
if (WIN32)
    target_link_libraries(sandbox
        opengl32
        gdi32
        user32
        shell32
        winmm
    )
elseif(APPLE)
    target_link_libraries(sandbox
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
else()
# Linux / Unix
    target_link_libraries(sandbox
        GL
        X11
        pthread
        dl
    )
endif()