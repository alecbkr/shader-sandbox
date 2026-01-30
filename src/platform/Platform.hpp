#pragma once

#include <types.hpp>
#include <memory>
#include "application/AppContext.hpp"
#include "platform/components/Window.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "core/input/ActionRegistry.hpp"

struct PlatformInitStruct {
    u32 width;
    u32 height;
    std::string title;
};

class Platform {
public:
    Platform();
    bool initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, Keybinds* _keybindsPtr, ActionRegistry* _actionRegistryPtr, u32 _width, u32 _height, const char* _app_title);
    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    void processInput();
    void initializeImGui();
    Window& getWindow();
    void initializeInputCallbacks();
    void setWindowIcon();
    double getTime();

private:
    bool initialized = false;
    std::unique_ptr<Window> windowPtr = nullptr;
    Logger* loggerPtr = nullptr;
    ContextManager* ctxManagerPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    ActionRegistry* actionRegPtr = nullptr;
};