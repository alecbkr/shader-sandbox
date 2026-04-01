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
    static void loadDefaultScene(AppContext& ctx);
private:
    static bool initialized;
    static bool shouldClose(AppContext& ctx);
    static void initializeUI(AppContext& ctx);
    static bool addDefaultActionBinds(ActionRegistry* actionRegPtr, ViewportUI* viewportUIPtr, ContextManager* contextManagerPtr, EventDispatcher* eventsPtr, Fonts* fontsPtr);
    static void addSubscriptions(AppContext& ctx);
    static std::string findNextFileNumber(const std::filesystem::path& baseFolder, const std::string& startingName);
};