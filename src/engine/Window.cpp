#include "Window.hpp"

#include "Errorlog.hpp"
#include "InputHandler.hpp"


Window::Window(const char *processName, int widthIn, int heightIn) {
    if (!glfwInit()) {
        ERRLOG.logEntry(EL_CRITICAL, "WINDOW", "glfwInit failure");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //newest version of openGL allowed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //oldest version of openGL allowed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //CORE(modern) or COMPATIBILITY(leg acy & modern)

    window = glfwCreateWindow(widthIn, heightIn, processName, NULL, NULL); 
    if (window == NULL) {
        glfwTerminate();
        ERRLOG.logEntry(EL_CRITICAL, "WINDOW", "glfwCreateWindow failure");
    }

    
    glfwMakeContextCurrent(window);
    // glfwSetKeyCallback(window, keyCallBackDefault); //TODO make control class

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        ERRLOG.logEntry(EL_CRITICAL, "WINDOW", "gladLoadGLLoader failure");
    }   
    
    glViewport(0, 0, widthIn, heightIn);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, InputHandler::key_callback);
    glfwSetMouseButtonCallback(window, InputHandler::mouse_callback);
    glfwSetCursorPosCallback(window, InputHandler::cursor_callback);
    glfwSetScrollCallback(window, InputHandler::scroll_callback);



    width = widthIn;
    height = heightIn;
    glfwSetWindowUserPointer(window, this); // used to retrieve width/height w/out them being static
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ERRLOG.logEntry(EL_INFO, "WINDOW", "Success");
}


void Window::swapBuffers() {
    glfwSwapBuffers(window);
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}


void Window::kill() {
    glfwDestroyWindow(window);
}

void Window::framebuffer_size_callback(GLFWwindow *window, int widthIn, int heightIn) {
    glViewport(0, 0, widthIn, heightIn);

    // Update window size variables
    Window *self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->width = widthIn;
    self->height = heightIn;
}