#pragma once

#include <types.hpp>
#include <memory>
#include <filesystem>
#include "platform/components/Window.hpp"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

class Logger;
class ContextManager;
class Keybinds;
class ActionRegistry;
class InputState;
class AppContext;
struct AppSettings;

struct WindowUserData {
    InputState* inputs;
    AppSettings* settings;
};

class Platform {
public:
    Platform();
    bool initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegistryPtr, InputState* _inputsPtr, const char* _app_title, AppSettings* settingsPtr);
    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    void processInput();
    void initializeImGui();
    Window& getWindow();
    void initializeInputCallbacks();
    void setWindowIcon();
    double getTime();
    std::filesystem::path getExeDir() const;
    void swapInterval(int interval);
    void iconifyWindow();
    void maximizeWindow();
    void moveWindowPosRelative(int x, int y);
    void getScreenCursorPosition(int& x, int& y) const;
    bool beginNativeWindowDrag();
    bool enableBorderlessSnap();
    void terminate();

private:
    bool initialized = false;
    std::unique_ptr<Window> windowPtr = nullptr;
    Logger* loggerPtr = nullptr;
    ContextManager* ctxManagerPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    ActionRegistry* actionRegPtr = nullptr;
    InputState* inputsPtr = nullptr;
    WindowUserData userData{};

    void installBorderlessWin32Hooks();
    void uninstallBorderlessWin32Hooks();
#ifdef _WIN32
    HWND hwnd = nullptr;
    WNDPROC oldWndProc = nullptr;
    static LRESULT CALLBACK WndProcThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    int resizeBorderPx = 8;
#endif
};