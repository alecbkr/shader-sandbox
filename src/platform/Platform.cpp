#include "platform/Platform.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>

bool Platform::initialized = false;
std::unique_ptr<Window> Platform::windowPtr = nullptr;

void setContextCurrent(Window& window) {
    window.setContextCurrent();
}

bool Platform::initialize(const PlatformInitStruct& initStruct) {
    if (Platform::initialized) {
        // TODO: use logger
        std::cout << "Platform layer is already initialized." << std::endl;    
        return false;
    }
    
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    bool windowIsValid;
    Platform::windowPtr = Window::createWindow(initStruct.width, initStruct.height, initStruct.title, windowIsValid);
    if (!windowIsValid) return false;
    setContextCurrent(*Platform::windowPtr);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    } 

    Platform::initialized = true;
    return true;
}

bool Platform::shouldClose() {
    return windowPtr->shouldClose();
}

void Platform::swapBuffers() {
    windowPtr->swapBuffers();
}

void Platform::pollEvents() {
    glfwPollEvents();
}

void Platform::processInput() {
    if (glfwGetKey(windowPtr->getGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(windowPtr->getGLFWWindow(), true);
    }
}

void Platform::initializeImGui() {
    ImGui_ImplGlfw_InitForOpenGL(windowPtr->getGLFWWindow(), true);
}