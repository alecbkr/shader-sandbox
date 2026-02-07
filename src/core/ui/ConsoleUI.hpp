#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <memory>
#include "../logging/ConsoleSink.hpp"
#include "../ConsoleEngine.hpp"

class Logger;

// Lookup table for textcolors for each log 
static const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(0.4f, 1.0f, 0.4f, 1.0f)          // Info (Light Green)
}; 

// Adapted from imgui_demo.cpp
class ConsoleUI {
public: 
    ConsoleUI();
    bool initialize(Logger* _loggerPtr);
    const void render();
     
private:
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
    
    std::shared_ptr<ConsoleEngine> engine = nullptr; 
    std::shared_ptr<ConsoleSink> logSrc = nullptr;

    // char inputBuf[256]; 
    std::vector<std::string> history; 
    // int historyPos; 

    bool isAutoScroll = false; 
    // bool isFocused = true;      // only take in input when the user is using the widget 
    size_t lastLogSize = 0; 

    bool initialized = false;
    
    const void drawConsole(); 
    void readLogs(); 
    const void drawMenuBar(); 

    // const void handleInputHistory(); // User input to go through logs in console 
    const void executeCommand(); 
}; 