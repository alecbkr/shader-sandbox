#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <memory>
#include <deque>
#include "../logging/Logger.hpp"
#include "../logging/ConsoleSink.hpp"
#include "../ConsoleEngine.hpp"
#include "engine/SearchText.hpp"

struct ConsoleBtns {
    bool isAutoScroll; 
    bool isCollapsedLogs; 
    bool isShowErrors; 
    bool isShowWarning; 
    bool isShowInfo; 
    // Filters/ShowSources menu 
    bool isShowShader; 
    bool isShowSystem; 
    bool isShowAssets;
};

// Lookup table for textcolors for each log 
static const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(0.4f, 1.0f, 0.4f, 1.0f)          // Info (Light Green)
                                            // Anomaly (light gray)
}; 

// Adapted from imgui_demo.cpp
class ConsoleUI {
public: 
    static bool initialize();
    static const void render();
    static SearchText searcher;
     
private: 
    struct LogStyle {
        std::string prefix; 
        ImVec4 color; 
    }; 

    // static std::shared_ptr<ConsoleEngine> engine; 
    // static std::shared_ptr<ConsoleSink> logSrc; 
    static float targetWidth;
    static float targetHeight;
    static ImVec2 windowPos;
    
    static std::shared_ptr<ConsoleEngine> engine; 
    static std::shared_ptr<ConsoleSink> logSrc; 

    static std::vector<std::string> history; 

    static std::vector<std::string> selectedLogIndices; 
    static int selectionStart; 
    static int selectionEnd; 

    static bool initialized;
    static ConsoleToggles &togStates; 

    static size_t lastLogSize; 
    static void drawLogs(); 
    static const void drawMenuBar(); 
    static void updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll); 
    static int getCollapseCount(const std::deque<LogEntry> &logs, int currIdx);
    static void drawSingleLog(const LogEntry& log, int index, int repeatCount, bool& isScroll);

    // Allows for users to copy their logs from the console (because of the way ImGui renders text I had to do this)
    static void copyLogsToClipboard(); 
    static LogStyle getLogStyle(const LogEntry& log); 
    static std::string formatLogString(const LogEntry& log); 
}; 