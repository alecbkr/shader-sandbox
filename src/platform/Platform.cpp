#include "platform/GL.hpp"
#include <imgui/imgui_impl_glfw.h>
#include <stb_image.h>
#include "platform/Platform.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/InputState.hpp"
#include "application/AppContext.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

Platform::Platform() {
    initialized = false;
    windowPtr = nullptr;
    loggerPtr = nullptr;
    ctxManagerPtr = nullptr;
    keybindsPtr = nullptr;
    actionRegPtr = nullptr;
    inputsPtr = nullptr;
}

void setContextCurrent(Window& window) {
    window.setContextCurrent();
}

void Platform::setWindowIcon() {
    int w = 0, h = 0, channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* pixels = stbi_load("../assets/icon.png", &w, &h, &channels, 4);
    if (!pixels) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "Platform Set Window Icon", "stbi didn't load the window icon.");
        return;
    }
    GLFWimage img;
    img.width = w;
    img.height = h;
    img.pixels = pixels;

    glfwSetWindowIcon(windowPtr->getGLFWWindow(), 1, &img);
    stbi_image_free(pixels);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    auto* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (!ctx) return;

    ctx->settings.width = static_cast<u32>(width);
    ctx->settings.height = static_cast<u32>(height);
}

void window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    auto* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
    if (!ctx) return;

    ctx->settings.posX = xpos;
    ctx->settings.posY = ypos;
}

bool Platform::initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegPtr, InputState* _inputsPtr, const char* _app_title, AppContext* ctxPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Platform Initialization", "The platform layer is already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    ctxManagerPtr = _ctxManagerPtr;
    keybindsPtr = _keybindsPtr;
    actionRegPtr = _actionRegPtr;
    inputsPtr = _inputsPtr;
    
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    bool windowIsValid;
    windowPtr = Window::createWindow(ctxPtr->settings.width, ctxPtr->settings.height, _app_title, windowIsValid);
    if (!windowIsValid) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform Window Creation", "Failed to create window.");
        return false;
    }

    
    glfwSetWindowUserPointer(windowPtr->getGLFWWindow(), inputsPtr);
    initializeInputCallbacks();
    
    setWindowIcon();
    setContextCurrent(*windowPtr);
    glfwSetWindowPos(windowPtr->getGLFWWindow(), ctxPtr->settings.posX, ctxPtr->settings.posY);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform GLAD Initialization", "Failed to initialize GLAD.");
        return false;
    }

    glViewport(0, 0, ctxPtr->settings.width, ctxPtr->settings.height);

    glfwSetWindowUserPointer(windowPtr->getGLFWWindow(), ctxPtr);
    glfwSetFramebufferSizeCallback(windowPtr->getGLFWWindow(), framebuffer_size_callback);
    glfwSetWindowPosCallback(windowPtr->getGLFWWindow(), window_position_callback);

    #ifdef __linux__
        // Disable vsync on linux because it can cause issues when shader sandbox window is minimized.
        loggerPtr->addLog(LogLevel::INFO, "Platform::initialize", "Need to add a feature to disable/enable vsync");
        glfwSwapInterval(0);
    #endif

    Platform::initialized = true;
    return true;
}

bool Platform::shouldClose() {
    if (!initialized) return true;
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
    // TODO: change call sites of this function and have it return a nullptr if not initialized and return a window* if it is
    assert(initialized && "Platform not initialized");
    return *windowPtr;
}

double Platform::getTime() {
    return glfwGetTime();
}

void Platform::initializeInputCallbacks() {
    GLFWwindow* w = windowPtr->getGLFWWindow();

    glfwSetKeyCallback(w, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto* in = static_cast<InputState*>(glfwGetWindowUserPointer(w));
        if (in) in->onKey(key, action);
    });

    glfwSetMouseButtonCallback(w, [](GLFWwindow* w, int button, int action, int mods) {
        auto* in = static_cast<InputState*>(glfwGetWindowUserPointer(w));
        if (in) in->onMouseButton(button, action);
    });

    glfwSetCursorPosCallback(w, [](GLFWwindow* w, double x, double y) {
        auto* in = static_cast<InputState*>(glfwGetWindowUserPointer(w));
        if (in) in->onCursorPos(x, y);
    });

    glfwSetScrollCallback(w, [](GLFWwindow* w, double x, double y) {
        auto* in = static_cast<InputState*>(glfwGetWindowUserPointer(w));
        if (in) in->onScroll(x, y);
    });
}

std::filesystem::path Platform::getExeDir() const {
#if defined(_WIN32)

    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();

#elif defined(__linux__)

    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }

#endif

    // Fallback (should rarely happen)
    return std::filesystem::current_path();
}

void Platform::terminate(){
    glfwTerminate();
}