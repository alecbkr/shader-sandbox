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
#include "components/SearchText.hpp"

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

class Logger;

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
    ConsoleUI();
    bool initialize(Logger* _loggerPtr);
    const void render();
    static SearchText searcher;
     
    struct LogStyle {
        std::string prefix;
        ImVec4 color;
    };

    // static std::shared_ptr<ConsoleEngine> engine;
    // static std::shared_ptr<ConsoleSink> logSrc;
private:
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
    
    std::shared_ptr<ConsoleEngine> engine = nullptr;
    std::shared_ptr<ConsoleSink> logSrc = nullptr;

    // char inputBuf[256]; 
    std::vector<std::string> history;
    // int historyPos; 

    std::vector<std::string> selectedLogIndices;
    int selectionStart;
    int selectionEnd;

    bool initialized = false;
    ConsoleToggles &togStates;
    Logger *loggerPtr = nullptr;

    size_t lastLogSize = 0;
    void drawLogs();
    const void drawMenuBar();
    void updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll);
    int getCollapseCount(const std::deque<LogEntry> &logs, int currIdx);
    void drawSingleLog(const LogEntry& log, int index, int repeatCount, bool& isScroll);

    // Allows for users to copy their logs from the console (because of the way ImGui renders text I had to do this)
    void copyLogsToClipboard();
    LogStyle getLogStyle(const LogEntry& log);
    std::string formatLogString(const LogEntry& log);
}; 