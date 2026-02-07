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
# TESTING (CTest)
# ---------------------------------------------------------
include(CTest)
enable_testing()

# ---------------------------------------------------------
# ADD GLFW (source version)
# ---------------------------------------------------------
add_subdirectory(lib/glfw)

# ASSIMP
add_subdirectory(lib/assimp)

# ---------------------------------------------------------
# RECURSIVE SOURCE COLLECTION
# ---------------------------------------------------------
file(GLOB_RECURSE ENGINE_CPP "${CMAKE_SOURCE_DIR}/src/*.cpp")

set(IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
)

set(STB_SRC ${CMAKE_SOURCE_DIR}/lib/stb/stb_image.cpp)
set(SRC_C   ${CMAKE_SOURCE_DIR}/lib/glad/glad.c)

# ---------------------------------------------------------
# ENGINE LIB (linked by sandbox + tests)
# ---------------------------------------------------------
add_library(sandbox_engine STATIC
    ${ENGINE_CPP}
    ${SRC_C}
    ${IMGUI_SRC}
    ${STB_SRC}
    ${ASSIMP_SRC}
)

target_include_directories(sandbox_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/stb
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib/glfw/include
)

target_link_libraries(sandbox_engine PUBLIC glfw assimp)

if (WIN32)
    target_link_libraries(sandbox_engine PUBLIC
        opengl32
        gdi32
        user32
        shell32
        winmm
    )
elseif (APPLE)
    target_link_libraries(sandbox_engine PUBLIC
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
else()
    target_link_libraries(sandbox_engine PUBLIC
        GL
        X11
        pthread
        dl
    )
endif()

# ---------------------------------------------------------
# APP EXECUTABLE
# ---------------------------------------------------------
add_executable(sandbox)

# ---- Windows RC in its own target (no include dirs passed to windres) ----
if (WIN32)
    add_library(sandbox_rc OBJECT ${CMAKE_SOURCE_DIR}/src/app.rc)
    set_target_properties(sandbox_rc PROPERTIES LINKER_LANGUAGE C)
    target_sources(sandbox PRIVATE $<TARGET_OBJECTS:sandbox_rc>)
endif()

target_link_libraries(sandbox PRIVATE sandbox_engine)

# ---------------------------------------------------------
# TEST EXECUTABLE (Catch2 amalgamated)
# ---------------------------------------------------------
file(GLOB_RECURSE TEST_CPP "${CMAKE_SOURCE_DIR}/tests/*.cpp")

add_executable(sandbox_tests
    ${TEST_CPP}
    ${CMAKE_SOURCE_DIR}/include/catch2/catch_amalgamated.cpp
)

target_include_directories(sandbox_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(sandbox_tests PRIVATE sandbox_engine)

add_test(NAME sandbox_tests COMMAND sandbox_tests)
