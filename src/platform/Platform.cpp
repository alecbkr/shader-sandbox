#include "platform/Platform.hpp"
#include <iostream>
#include <GLFW/glfw3.h>

bool Platform::initialized = false;
Window Platform::window;

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
    window = Window::createWindow(initStruct.width, initStruct.height, initStruct.title, windowIsValid);
    if (!windowIsValid) return false;
    setContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    } 

    Platform::initialized = true;
    return true;
}

bool Platform::shouldClose() {
    return window.shouldClose();
}

void Platform::swapBuffers() {
    window.swapBuffers();
}

void Platform::pollEvents() {
    glfwPollEvents();
}

void Platform::processInput() {
    if (glfwGetKey(window.getGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window.getGLFWWindow(), true);
    }
}