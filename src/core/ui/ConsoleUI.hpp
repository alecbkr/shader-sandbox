#pragma once 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <memory>
#include <deque>

#include "../logging/ConsoleSink.hpp"
#include "../ConsoleEngine.hpp"
#include "components/SearchText.hpp"

class Logger;

class ConsoleUI {
public: 
    ConsoleUI();
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