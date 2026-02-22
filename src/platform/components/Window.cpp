#include "platform/components/Window.hpp"
#include <iostream>

std::unique_ptr<Window> Window::createWindow(u32 width, u32 height, std::string title, bool& outIsValid) {
    std::unique_ptr<Window> windowPtr = std::make_unique<Window>(width, height, title);
    outIsValid = windowPtr->checkValidity();
    return windowPtr;
}

Window::Window(u32 _width, u32 _height, std::string _title) : width(_width), height(_height), title(_title) {
    window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
    if (window == NULL) {
        // TODO: use logger
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        isValid = false;
        return;
    }

    glfwMakeContextCurrent(window);
    isValid = true;
}

Window::Window() {};

bool Window::checkValidity() {
    return isValid;
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::setContextCurrent() {
    glfwMakeContextCurrent(window);
}

GLFWwindow* Window::getGLFWWindow() {
    if (!window) return nullptr;
    return window;
}