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
#include "components/TextSelector.hpp"

class Logger;

class ConsoleUI {
public: 
    ConsoleUI();
    bool initialize(Logger* _loggerPtr, ConsoleEngine* _engine);
    void render();

    struct LogStyle {
        std::string prefix;
        ImVec4 color;
    };

    struct DisplayLine {
        int originalLogIdx; 
        std::string text; 
        bool isWrap; 
        int collapsedCount; 
        int charOffset; 
    };

private:
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
    
    ConsoleEngine *engine = nullptr;
    std::shared_ptr<ConsoleSink> logSrc = nullptr;
    Logger* loggerPtr; 
    SearchText searcher;
    TextSelectionCtx selectionCtx; 
    TextSelectorLayout selectionLayout; 

    size_t lastLogSize = 0;
    bool initialized = false;
    int selectionStart = -1;

    void drawLogs();
    void drawMenuBar();
    void updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll);
    int getCollapseCount(const std::deque<LogEntry> &logs, int currIdx);
    void drawSingleLog(int rowIdx, const DisplayLine& lineData, const LogEntry& originalLog, bool& isScroll);

    // helpers 
    LogStyle getLogStyle(const LogEntry& log); 
    std::string formatLogString(const LogEntry& log); 
    bool isLogFiltered(const LogEntry& log); 
    std::vector<DisplayLine> wrapLogText(const std::string& fullText, int logIndex, int collapseCount, float maxWidth);
};