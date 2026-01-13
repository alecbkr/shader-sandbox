#pragma once

#include "types.hpp"
#include <string>
#include "core/logging/Logger.hpp"

struct ApplicationInitStruct {
    u32 width;
    u32 height;
    std::string title;
    LoggerInitialization loggerSetting;
};

class Application {
public:
    static bool initialize(const ApplicationInitStruct& initStruct);
    static void runLoop();
    static void renderUI();
    static void shutdown();

private:
    static bool initialized;
    static bool shouldClose();
};