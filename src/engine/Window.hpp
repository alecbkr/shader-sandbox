#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
    public:
        GLFWwindow *window;
        Window(const char *processName, int width, int height);
        void swapBuffers();
        bool shouldClose();
        void kill();
};

#endif