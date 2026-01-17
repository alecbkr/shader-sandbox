#include "Window.hpp"

// #include "Errorlog.hpp"
#include "core/logging/Logger.hpp"
#include "InputHandler.hpp"


OldWindow::OldWindow(const char *processName, int widthIn, int heightIn) {
    if (!glfwInit()) {
        Logger::addLog(LogLevel::CRITICAL, "WINDOW", "glfwInit failure"); 
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //newest version of openGL allowed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //oldest version of openGL allowed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //CORE(modern) or COMPATIBILITY(leg acy & modern)

    window = glfwCreateWindow(widthIn, heightIn, processName, NULL, NULL); 
    if (window == NULL) {
        glfwTerminate();
        Logger::addLog(LogLevel::CRITICAL, "WINDOW", "glfwCreateWindow failure"); 
    }

    
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        Logger::addLog(LogLevel::CRITICAL, "WINDOW", "gladLoadGLLoader failure"); 
    }   
    
    // WINDOWSIZE.width = widthIn;
    // WINDOWSIZE.height = heightIn;
    
    // glfwSetKeyCallback(window, InputHandler::key_callback);
    // glfwSetMouseButtonCallback(window, InputHandler::mouse_callback);
    // glfwSetCursorPosCallback(window, InputHandler::cursor_callback);
    // glfwSetScrollCallback(window, InputHandler::scroll_callback);
    // glfwSetFramebufferSizeCallback(window, InputHandler::windowSize_callback);


    // glViewport(0, 0, WINDOWSIZE.width, WINDOWSIZE.height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // ERRLOG.logEntry(EL_INFO, "WINDOW", "Success");
    // Logger::addLog(LogLevel::INFO, "WINDOW", "Success"); 
}


void OldWindow::swapBuffers() {
    glfwSwapBuffers(window);
}


bool OldWindow::shouldClose() {
    return glfwWindowShouldClose(window);
}


void OldWindow::destroy() {
    glfwDestroyWindow(window);
}