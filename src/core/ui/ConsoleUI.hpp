#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <memory>
#include "../logging/ConsoleSink.hpp"
#include "../ConsoleEngine.hpp"

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
    static bool initialize(std::shared_ptr<ConsoleSink> consoleSink);
    static const void render();
     
private:
    static float targetWidth;
    static float targetHeight;
    static ImVec2 windowPos;
    
    static std::shared_ptr<ConsoleEngine> engine; 
    static std::shared_ptr<ConsoleSink> logSrc; 

    // char inputBuf[256]; 
    static std::vector<std::string> history; 
    // int historyPos; 

    static bool isAutoScroll; 
    // bool isFocused = true;      // only take in input when the user is using the widget 
    static size_t lastLogSize; 

    static bool initialized;
    
    static const void drawConsole(); 
    static void readLogs(); 
    static const void drawMenuBar(); 

    // const void handleInputHistory(); // User input to go through logs in console 
    const void executeCommand(); 
}; 