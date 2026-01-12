#include "application/Application.hpp"
#include "platform/Platform.hpp"
#include <iostream>

bool Application::initialized = false;

bool Application::initialize(const ApplicationInitStruct& initStruct) {
    if (Application::initialized) {
        std::cout << "Application layer already initialized." << std::endl;
        return false;
    }

    // Initialize the logger
    if (!Logger::initialize(initStruct.loggerSetting)) {
        std::cout << "Logger was not initialized successfully." << std::endl;
        return false;
    }

    if (!Platform::initialize({initStruct.width, initStruct.height, initStruct.title})) {
        std::cout << "Platform layer was not initialized successfully." << std::endl;
        return false;
    }
    
    Logger::addLog(LogLevel::INFO, "Application Initialization", "Application Layer Initialized.");
    Application::initialized = true;
    return true;
}

void Application::runLoop() {
    if (!Application::initialized) {
        std::cout << "Attempting to run render loop without initializing application layer." << std::endl;
        return;
    }
}