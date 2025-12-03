cmake_minimum_required(VERSION 3.20)
project(MyProject C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall")

# Output to ./bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------------------------------------------
# ADD GLFW (source version)
# ---------------------------------------------------------
add_subdirectory(lib/glfw)

# GLFW exposes this include path
include_directories(lib/glfw/include)

# ---------------------------------------------------------
# INCLUDE PROJECT DIRECTORIES
# ---------------------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/stb
)

# ---------------------------------------------------------
# RECURSIVE SOURCE COLLECTION
# ---------------------------------------------------------
file(GLOB_RECURSE SRC_CPP
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

set(IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
)


set(SRC_C 
    "${CMAKE_SOURCE_DIR}/lib/glad/glad.c"
)

# ---------------------------------------------------------
# EXECUTABLE
# ---------------------------------------------------------
add_executable(sandbox
    ${SRC_CPP}
    ${SRC_C}
    ${IMGUI_SRC}
)

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
else() # Linux / Unix
    target_link_libraries(sandbox
        GL
        X11
        pthread
        dl
    )
endif()