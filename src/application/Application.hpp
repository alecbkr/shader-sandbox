#pragma once

#include "types.hpp"
#include <string>
#include "application/AppContext.hpp"
#include "core/logging/Logger.hpp"

struct ApplicationInitStruct {
    u32 width;
    u32 height;
    std::string title;
    LoggerInitialization loggerSetting;
};

// enum AppStateControls {
//     NO_STATE,
//     AS_EDITOR,
//     AS_CAMERA
// };

class Application {
public:
    static bool initialize(AppContext& ctx);
    static void runLoop();
    static void renderUI();
    static void shutdown();
    // static void setAppStateControls(AppStateControls state);
    // static AppStateControls checkAppStateControls();

private:
    static bool initialized;
    // static AppStateControls appControls;
    static bool shouldClose();
};