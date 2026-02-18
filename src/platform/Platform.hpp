#pragma once

#include <types.hpp>
#include <memory>
#include <filesystem>
#include "platform/components/Window.hpp"

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
};