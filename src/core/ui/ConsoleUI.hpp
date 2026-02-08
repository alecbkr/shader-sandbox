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
#include "components/TextSelector.hpp"

class ConsoleUI {
public: 
    static bool initialize();
    static const void render();
    static SearchText searcher;
     
    struct LogStyle {
        std::string prefix; 
        ImVec4 color; 
    }; 

private:
static bool initialized;
    static float targetWidth;
    static float targetHeight;
    static ImVec2 windowPos;
    
    static std::shared_ptr<ConsoleEngine> engine; 
    static std::shared_ptr<ConsoleSink> logSrc; 
    static ConsoleToggles &togStates; 

    static TextSelectionCtx selection; 
    static std::vector<int> filteredIndices; 
    static size_t lastLogSize; 

    static void drawLogs(); 
    static const void drawMenuBar(); 
    static void updateSearchAndScroll(const std::deque<LogEntry> &logs); 
    static int getCollapseCount(const std::deque<LogEntry> &logs, int currIdx);

    static void copySelectedLogs(); 
    static LogStyle getLogStyle(const LogEntry& log); 
    static std::string formatLogString(const LogEntry& log);  
}; 