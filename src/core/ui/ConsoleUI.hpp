#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <string>
#include <vector>
#include <memory>
#include <deque>
#include "../logging/ConsoleSink.hpp"
#include "../ConsoleEngine.hpp"

// Lookup table for textcolors for each log 
static const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),         // Info (White)
}; 

// Adapted from imgui_demo.cpp
class ConsoleUI {
public: 
    static bool initialize(std::shared_ptr<ConsoleSink> consoleSink);
    static const void render();
     
private: 
    struct LogStyle {
        std::string prefix; 
        ImVec4 color; 
    }; 

    static std::shared_ptr<ConsoleEngine> engine; 
    static std::shared_ptr<ConsoleSink> logSrc; 
    static std::vector<std::string> history; 

    static std::vector<std::string> selectedLogIndices; 
    static int selectionStart; 
    static int selectionEnd; 

    static bool initialized;
    static bool isAutoScroll; 

    static size_t lastLogSize; 
    static void drawLogs(); 
    static const void drawMenuBar(); 

    // Allows for users to copy their logs from the console (because of the way ImGui renders text I had to do this)
    static void copyLogsToClipboard(); 
    static const void executeCommand(); 
    static LogStyle getLogStyle(const LogEntry& log); 
    static std::string formatLogString(const LogEntry& log); 
}; 