#pragma once

#include "imgui/imgui.h"
#include "core/MenuEngine.hpp"

class Logger;
class EventDispatcher;

class MenuUI {
public:
    MenuUI();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr);
    void render();

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    void drawMenuBar();
    void drawMenuItem(const MenuItem& item);
};
