#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <memory>
#include "types.hpp"

class Window {
public:
    static std::unique_ptr<Window> createWindow(u32 width, u32 height, std::string title, bool& outIsValid);

    Window(u32 _width, u32 _height, std::string _title);
    Window();
    bool checkValidity ();
    bool shouldClose();
    void swapBuffers();
    void setContextCurrent();
    GLFWwindow* getGLFWWindow();

private:
    u32 width;
    u32 height;
    std::string title;

    GLFWwindow* window;
    bool isValid;
};