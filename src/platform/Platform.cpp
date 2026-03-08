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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <windowsx.h>
#include <GLFW/glfw3native.h>
static constexpr wchar_t kPlatformPropName[] = L"ShaderSandbox.PlatformPtr";
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
    userData.inputs = nullptr;
    userData.settings = nullptr;
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

    auto* settings = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window))->settings;
    if (!settings) return;

    settings->width = static_cast<u32>(width);
    settings->height = static_cast<u32>(height);
}

void window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    auto* settings = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window))->settings;
    if (!settings) return;

    settings->posX = xpos;
    settings->posY = ypos;
}

bool Platform::initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegPtr, InputState* _inputsPtr, const char* _app_title, AppSettings* settingsPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Platform Initialization", "The platform layer is already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    ctxManagerPtr = _ctxManagerPtr;
    keybindsPtr = _keybindsPtr;
    actionRegPtr = _actionRegPtr;
    inputsPtr = _inputsPtr;
    userData.inputs = _inputsPtr;
    userData.settings = settingsPtr;
    
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    bool windowIsValid;
    windowPtr = Window::createWindow(settingsPtr->width, settingsPtr->height, _app_title, windowIsValid);
    if (!windowIsValid) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform Window Creation", "Failed to create window.");
        return false;
    }

#ifdef _WIN32
    enableBorderlessSnap();
    installBorderlessWin32Hooks();
#endif
    
    glfwSetWindowUserPointer(windowPtr->getGLFWWindow(), &userData);
    initializeInputCallbacks();
    
    setWindowIcon();
    setContextCurrent(*windowPtr);
    glfwSetWindowPos(windowPtr->getGLFWWindow(), settingsPtr->posX, settingsPtr->posY);
    glfwSwapInterval(settingsPtr->vsyncEnabled ? 1 : 0);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        loggerPtr->addLog(LogLevel::CRITICAL, "Platform GLAD Initialization", "Failed to initialize GLAD.");
        return false;
    }
    
    glViewport(0, 0, settingsPtr->width, settingsPtr->height);
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
        auto* in = static_cast<WindowUserData*>(glfwGetWindowUserPointer(w))->inputs;
        if (in) in->onKey(key, action);
    });

    glfwSetMouseButtonCallback(w, [](GLFWwindow* w, int button, int action, int mods) {
        auto* in = static_cast<WindowUserData*>(glfwGetWindowUserPointer(w))->inputs;
        if (in) in->onMouseButton(button, action);
    });

    glfwSetCursorPosCallback(w, [](GLFWwindow* w, double x, double y) {
        auto* in = static_cast<WindowUserData*>(glfwGetWindowUserPointer(w))->inputs;
        if (in) in->onCursorPos(x, y);
    });

    glfwSetScrollCallback(w, [](GLFWwindow* w, double x, double y) {
        auto* in = static_cast<WindowUserData*>(glfwGetWindowUserPointer(w))->inputs;
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

void Platform::swapInterval(int interval) {
    glfwSwapInterval(interval);
}

void Platform::iconifyWindow() {
    glfwIconifyWindow(windowPtr->getGLFWWindow());
}

void Platform::maximizeWindow() {
    glfwMaximizeWindow(windowPtr->getGLFWWindow());
}

void Platform::moveWindowPosRelative(int x, int y) {
    int wx, wy;
    glfwGetWindowPos(windowPtr->getGLFWWindow(), &wx, &wy);
    glfwSetWindowPos(windowPtr->getGLFWWindow(), wx + x, wy + y);
}

void Platform::getScreenCursorPosition(int& x, int& y) const {
    GLFWwindow* w = windowPtr->getGLFWWindow();

    int winX, winY;
    glfwGetWindowPos(w, &winX, &winY);

    double cursorX, cursorY;
    glfwGetCursorPos(w, &cursorX, &cursorY);

    x = winX + cursorX;
    y = winY + cursorY;
}

bool Platform::beginNativeWindowDrag()
{
#ifdef _WIN32
    GLFWwindow* w = windowPtr->getGLFWWindow();
    HWND hwnd = glfwGetWin32Window(w);

    ReleaseCapture();
    SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    glfwPollEvents();

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(0, false);

    int sx, sy;
    getScreenCursorPosition(sx, sy);

    io.AddMousePosEvent((float)sx, (float)sy);
    ImGui::ClearActiveID();

    return true;
#endif
    return false;
}

bool Platform::enableBorderlessSnap() {
#ifdef _WIN32
    GLFWwindow* w = windowPtr->getGLFWWindow();
    HWND hwnd = glfwGetWin32Window(w);

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

    style &= ~(WS_CAPTION);
    style |=  (WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    return true;
#else
    return false;
#endif
}

void Platform::setCursorStatus(bool status) {
    if (status) glfwSetInputMode(windowPtr->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode(windowPtr->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Platform::terminate(){
    uninstallBorderlessWin32Hooks();
    glfwTerminate();
}

void Platform::installBorderlessWin32Hooks(){
#ifdef _WIN32
    GLFWwindow* w = windowPtr->getGLFWWindow();
    hwnd = glfwGetWin32Window(w);
    SetPropW(hwnd, kPlatformPropName, (HANDLE)this);
    oldWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)&Platform::WndProcThunk);
#endif
}

void Platform::uninstallBorderlessWin32Hooks(){
#ifdef _WIN32
    if (!hwnd) return;
    if (oldWndProc) SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)oldWndProc);
    RemovePropW(hwnd, kPlatformPropName);
    hwnd = nullptr;
    oldWndProc = nullptr;
#endif
}

#ifdef _WIN32
LRESULT CALLBACK Platform::WndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* self = (Platform*)GetPropW(hWnd, kPlatformPropName);
    if (self) return self->wndProc(hWnd, msg, wParam, lParam);
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
#endif

#ifdef _WIN32
LRESULT Platform::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCALCSIZE: {
            if (wParam == TRUE)
                return 0;
            break;
        }
        
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;

            HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = {};
            mi.cbSize = sizeof(mi);

            if (GetMonitorInfoW(monitor, &mi)) {
                RECT work = mi.rcWork;
                RECT mon  = mi.rcMonitor;

                mmi->ptMaxPosition.x = work.left - mon.left;
                mmi->ptMaxPosition.y = work.top  - mon.top;
                mmi->ptMaxSize.x     = work.right  - work.left;
                mmi->ptMaxSize.y     = work.bottom - work.top;
            }
            return 0;
        }

        case WM_NCHITTEST: {
            LRESULT hit = CallWindowProcW(oldWndProc, hWnd, msg, wParam, lParam);
            if (hit != HTCLIENT) return hit;
            if (IsZoomed(hWnd)) return HTCLIENT;

            POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RECT r; GetWindowRect(hWnd, &r);

            const int b = resizeBorderPx;

            const bool left   = p.x >= r.left && p.x < r.left + b;
            const bool right  = p.x <  r.right && p.x >= r.right - b;
            const bool top    = p.y >= r.top && p.y < r.top + b;
            const bool bottom = p.y <  r.bottom && p.y >= r.bottom - b;

            if (top && left)     return HTTOPLEFT;
            if (top && right)    return HTTOPRIGHT;
            if (bottom && left)  return HTBOTTOMLEFT;
            if (bottom && right) return HTBOTTOMRIGHT;
            if (left)            return HTLEFT;
            if (right)           return HTRIGHT;
            if (top)             return HTTOP;
            if (bottom)          return HTBOTTOM;

            return HTCLIENT;
        }
    }

    return CallWindowProcW(oldWndProc, hWnd, msg, wParam, lParam);
}
#endif