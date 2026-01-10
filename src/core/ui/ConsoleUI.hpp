#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
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
    ConsoleUI(std::shared_ptr<ConsoleSink> consoleSink);    
    ~ConsoleUI();  

    ConsoleUI(const ConsoleUI&)  = delete; 
    ConsoleUI& operator = (const ConsoleUI&) = delete; 

    const void render();
     
    private: 
    std::shared_ptr<ConsoleEngine> engine; 
    std::shared_ptr<ConsoleSink> logSrc; 

    // char inputBuf[256]; 
    std::vector<std::string> history; 
    // int historyPos; 

    bool isAutoScroll = false; 
    // bool isFocused = true;      // only take in input when the user is using the widget 
    size_t lastLogSize = 0; 
    
    const void drawConsole(); 
    void readLogs(); 
    const void drawMenuBar(); 

    // const void handleInputHistory(); // User input to go through logs in console 
    const void executeCommand(); 
}; 