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

struct PlatformInitStruct {
    u32 width;
    u32 height;
    std::string title;
};

class Platform {
public:
    Platform();
    bool initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegistryPtr, InputState* _inputsPtr, u32 _width, u32 _height, const char* _app_title);
    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    void processInput();
    void initializeImGui();
    Window& getWindow();
    void initializeInputCallbacks();
    void setWindowIcon();
    double getTime();
    std::filesystem::path getExeDir();

private:
    bool initialized = false;
    std::unique_ptr<Window> windowPtr = nullptr;
    Logger* loggerPtr = nullptr;
    ContextManager* ctxManagerPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    ActionRegistry* actionRegPtr = nullptr;
    InputState* inputsPtr = nullptr;
};