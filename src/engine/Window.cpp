#include "Window.hpp"

#include "Errorlog.hpp"


Window::Window(const char *processName, int width, int height) {
    if (!glfwInit()) {
        ERRLOG.logEntry(critical, "WINDOW", "glfwInit failure");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //newest version of openGL allowed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //oldest version of openGL allowed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //CORE(modern) or COMPATIBILITY(leg acy & modern)

    window = glfwCreateWindow(width, height, processName, NULL, NULL); 
    if (window == NULL) {
        glfwTerminate();
        ERRLOG.logEntry(critical, "WINDOW", "glfwCreateWindow failure");
    }

    
    glfwMakeContextCurrent(window);
    // glfwSetKeyCallback(window, keyCallBackDefault); //TODO make control class

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        ERRLOG.logEntry(critical, "WINDOW", "gladLoadGLLoader failure");
    }   
    
    // Additional settings
    // glViewport(0, 0, width, height);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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