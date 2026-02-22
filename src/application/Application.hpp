#pragma once

#include "types.hpp"
#include <string>
#include "application/AppContext.hpp"
#include "core/logging/Logger.hpp"
#include <imgui/imgui.h>
#include <array>

class Application {
public:
    static bool initialize(AppContext& ctx);
    static void runLoop(AppContext& ctx);
    static void renderUI(AppContext& ctx);
    static void shutdown(AppContext& ctx);
    static void windowResize(AppContext& ctx, u32 width, u32 height);
private:
    static bool initialized;
    static std::array<ImFont*, 6> fonts;
    static std::size_t fontIdx;
    static bool shouldClose(AppContext& ctx);
    static void initializeUI(AppContext& ctx);
    static bool addDefaultActionBinds(ActionRegistry* actionRegPtr, ViewportUI* viewportUIPtr, ContextManager* contextManagerPtr, EventDispatcher* eventsPtr);
    static void increaseFont();
    static void decreaseFont();
};