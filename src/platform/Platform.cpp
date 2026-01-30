#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <stb_image.h>
#include "platform/Platform.hpp"

Platform::Platform() {
    initialized = false;
    windowPtr = nullptr;
    loggerPtr = nullptr;
    ctxManagerPtr = nullptr;
    keybindsPtr = nullptr;
    actionRegPtr = nullptr;
}

void setContextCurrent(Window& window) {
    window.setContextCurrent();
}

void Platform::setWindowIcon() {
    int w = 0, h = 0, channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* pixels = stbi_load("../assets/icon.png", &w, &h, &channels, 4);
    if (!pixels) {
        loggerPtr->addLog(LogLevel::ERROR, "Platform Set Window Icon", "stbi didn't load the window icon.");
        return;
    }
    GLFWimage img;
    img.width = w;
    img.height = h;
    img.pixels = pixels;

    glfwSetWindowIcon(windowPtr->getGLFWWindow(), 1, &img);
    stbi_image_free(pixels);
}

bool Platform::initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegPtr, u32 _width, u32 _height, const char* _app_title) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Platform Initialization", "The platform layer is already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    ctxManagerPtr = _ctxManagerPtr;
    keybindsPtr = _keybindsPtr;
    actionRegPtr = _actionRegPtr;
    
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    bool windowIsValid;
    windowPtr = Window::createWindow(_width, _height, _app_title, windowIsValid);
    if (!windowIsValid) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform Window Creation", "Failed to create window.");
        return false;
    }

    setWindowIcon();
    setContextCurrent(*windowPtr);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform GLAD Initialization", "Failed to initialize GLAD.");
        return false;
    } 

    Platform::initialized = true;
    return true;
}

bool Platform::shouldClose() {
    if (!initialized) return;
    return windowPtr->shouldClose();
}

void Platform::swapBuffers() {
    if (!initialized) return;
    windowPtr->swapBuffers();
}

void Platform::pollEvents() {
    if (!initialized) return;
    glfwPollEvents();
}

void Platform::processInput() {
    if (!initialized) return;
    keybindsPtr->gatherActionsForFrame(ctxManagerPtr->current());
    actionRegPtr->processActionsForFrame();
}

void Platform::initializeImGui() {
    ImGui_ImplGlfw_InitForOpenGL(windowPtr->getGLFWWindow(), true);
}

Window& Platform::getWindow() {
    if (!initialized) return;
    return *windowPtr;
}

double Platform::getTime() {
    return glfwGetTime();
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