cmake_minimum_required(VERSION 3.20)
project(MyProject C CXX)

# ---------------------------------------------------------
# COMPILER SETTINGS
# ---------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall")

# Output executable in ./bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------------------------------------------
# INCLUDE DIRECTORIES
# ---------------------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/imgui
)

# ---------------------------------------------------------
# FIND SOURCES RECURSIVELY
# ---------------------------------------------------------
file(GLOB_RECURSE SRC_CPP
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

set(SRC_C 
    "${CMAKE_SOURCE_DIR}/lib/glad/glad.c"
)

# ---------------------------------------------------------
# CREATE EXECUTABLE
# ---------------------------------------------------------
add_executable(build
    ${SRC_CPP}
    ${SRC_C}
)

# ---------------------------------------------------------
# LINKER SETTINGS
# ---------------------------------------------------------
if (WIN32)
    target_link_libraries(build
        ${CMAKE_SOURCE_DIR}/lib/glfw/libglfw3dll.a
        opengl32
        gdi32
        user32
        shell32
        winmm
    )

elseif (APPLE)
    target_link_libraries(build
        ${CMAKE_SOURCE_DIR}/lib/libglfw.3.dylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
    )

elseif (UNIX)
    target_link_libraries(build
        glfw
        GL
        X11
        pthread
        dl
    )
endif()