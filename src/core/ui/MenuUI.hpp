#pragma once

#include "imgui/imgui.h"
#include "core/MenuEngine.hpp"
#include "core/ui/modals/ModalManager.hpp"

class Logger;
class EventDispatcher;
class Keybinds;
class Platform;
struct AppContext;

class MenuUI {
public:
    MenuUI();
    bool initialize(Logger* _loggerPtr, Platform* _platformPtr, EventDispatcher* _eventsPtr, ModalManager* _modalsPtr, Keybinds* _keybindsPtr, AppContext* ctx);
    void render();

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModalManager* modalsPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    Platform* platformPtr = nullptr;
    AppContext* appctx = nullptr;
    void drawMenuBar();
    void drawMenuItem(const MenuItem& item);
};
