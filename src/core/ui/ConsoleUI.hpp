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

class Logger;

// Lookup table for text colors for each log level
const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(0.4f, 1.0f, 0.4f, 1.0f)          // Info (Light Green)
                                            // Anomaly (light gray)
}; 

class ConsoleUI {
public: 
    ConsoleUI() = default;
    bool initialize(Logger* _loggerPtr);
    void render();

    struct LogStyle {
        std::string prefix; 
        ImVec4 color; 
    }; 

private:
    SearchText searcher;

    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
    
    std::shared_ptr<ConsoleEngine> engine = nullptr; 
    std::shared_ptr<ConsoleSink> logSrc = nullptr;
    Logger* loggerPtr; 

    size_t lastLogSize = 0; 
    bool initialized = false;
    int selectionStart = -1; 
    
    void drawLogs(); 
    void drawMenuBar(); 
    void updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll); 
    int getCollapseCount(const std::deque<LogEntry> &logs, int currIdx);
    void drawSingleLog(const LogEntry& log, int index, int repeatCount, bool& isScroll);

    // Allows for users to copy their logs from the console
    void copyLogsToClipboard(); 
    LogStyle getLogStyle(const LogEntry& log); 
    std::string formatLogString(const LogEntry& log); 
};