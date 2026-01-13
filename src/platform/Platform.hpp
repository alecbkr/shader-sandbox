#pragma once

#include <glad/glad.h>
#include <memory>
#include "platform/components/Window.hpp"

struct PlatformInitStruct {
    u32 width;
    u32 height;
    std::string title;
};

class Platform {
public:
    static bool initialize(const PlatformInitStruct& initStruct);
    static bool shouldClose();
    static void swapBuffers();
    static void pollEvents();
    static void processInput();
    static void initializeImGui();

private:
    static bool initialized;
    static std::unique_ptr<Window> windowPtr;
};