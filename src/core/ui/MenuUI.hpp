#pragma once

#include "imgui/imgui.h"
#include "core/MenuEngine.hpp"
#include "core/ui/modals/ModalManager.hpp"

class Logger;
class EventDispatcher;

class MenuUI {
public:
    MenuUI();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModalManager* _modalsPtr);
    void render();

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModalManager* modalsPtr = nullptr;
    void drawMenuBar();
    void drawMenuItem(const MenuItem& item);
};
