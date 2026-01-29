#include "platform/Platform.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <stb_image.h>
//#include "core/input/InputState.hpp"
#include "core/input/Keybinds.hpp"
#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"

bool Platform::initialized = false;
std::unique_ptr<Window> Platform::windowPtr = nullptr;

void setContextCurrent(Window& window) {
    window.setContextCurrent();
}

void Platform::setWindowIcon() {
    int w = 0, h = 0, channels = 0;
    unsigned char* pixels = stbi_load("../assets/icon.png", &w, &h, &channels, 4);
    if (!pixels) {
        std::cout << "stbi didnt load the window icon" << std::endl;
        return;
    }
    GLFWimage img;
    img.width = w;
    img.height = h;
    img.pixels = pixels;

    glfwSetWindowIcon(Platform::windowPtr->getGLFWWindow(), 1, &img);
    stbi_image_free(pixels);
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
    setWindowIcon();
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
    Keybinds::gatherActionsForFrame(ContextManager::current());
    ActionRegistry::processActionsForFrame();
}

void Platform::initializeImGui() {
    ImGui_ImplGlfw_InitForOpenGL(windowPtr->getGLFWWindow(), true);
}

Window& Platform::getWindow() {
    if (!initialized) Logger::addLog(LogLevel::WARNING, "Platform::getWindow()", "Attempting to return the application's window before initialization.");
    return *Platform::windowPtr;
}

void keyCallback(GLFWwindow*, int key, int, int action, int) { InputState::onKey(key, action); }
void mouseCallback(GLFWwindow*, int button, int action, int) { InputState::onMouseButton(button, action); }
void cursorCallback(GLFWwindow*, double x, double y) { InputState::onCursorPos(x, y); }
void scrollCallback(GLFWwindow*, double x, double y) { InputState::onScroll(x, y); }
void Platform::initializeInputCallbacks() {
    glfwSetKeyCallback(windowPtr->getGLFWWindow(), keyCallback);
    glfwSetMouseButtonCallback(windowPtr->getGLFWWindow(), mouseCallback);
    glfwSetCursorPosCallback(windowPtr->getGLFWWindow(), cursorCallback);
    glfwSetScrollCallback(windowPtr->getGLFWWindow(), scrollCallback);
}