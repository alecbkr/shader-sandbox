#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
    public:
        GLFWwindow *window;
        int width;
        int height;
        Window(const char *processName, int widthIn, int heightIn);
        void swapBuffers();
        bool shouldClose();
        void kill();
    
    private:
        static void framebuffer_size_callback(GLFWwindow *window, int widthIn, int heightIn);
};

#endif