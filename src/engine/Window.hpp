#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OldWindow {
    public:
        GLFWwindow *window;
        OldWindow(const char *processName, int widthIn, int heightIn);
        void swapBuffers();
        bool shouldClose();
        void destroy();
};

#endif