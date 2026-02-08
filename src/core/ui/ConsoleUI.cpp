#include "ConsoleUI.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include "platform/Platform.hpp"
#include <limits>

bool ConsoleUI::initialized = false; 
size_t ConsoleUI::lastLogSize = 0;
float ConsoleUI::targetWidth = 0.0f;
float ConsoleUI::targetHeight = 0.0f;
ImVec2 ConsoleUI::windowPos = ImVec2(0, 0);
SearchText ConsoleUI::searcher; 
ConsoleToggles& ConsoleUI::togStates = ConsoleEngine::getToggles();
TextSelectionCtx ConsoleUI::selection; 
std::vector<int> ConsoleUI::filteredIndices; 

// Lookup table for textcolors for each log 
static const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(0.4f, 1.0f, 0.4f, 1.0f)          // Info (Light Green)
                                            // Anomaly (light gray)
}; 

bool ConsoleUI::initialize() {
    searcher.setSearchFlag(SearchUIFlags::ADVANCED); 

    initialized = true;
    return true;
}
 
// change this to draw the entire componenet 
const void ConsoleUI::render() {
    if (!initialized) return;
    float menuBarHeight = ImGui::GetFrameHeight();
    
    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    ConsoleUI::targetWidth = (float)displayWidth * 0.4f;
    ConsoleUI::targetHeight = (float)displayHeight * 0.3f;
    
    int editorOffsetY = displayHeight * 0.7f;
    
    ConsoleUI::windowPos.x = 0;
    ConsoleUI::windowPos.y = editorOffsetY + menuBarHeight;

    ImGui::SetNextWindowSize(ImVec2(targetWidth, targetHeight + 1), ImGuiCond_Always);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

    if(ImGui::Begin("Console", nullptr, flags)) {  
        ConsoleUI::drawMenuBar();
        {

            ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
            ConsoleUI::drawLogs(); 

            // TODO: see if we can integrate this with InputState or KeyBinds 
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
                    copySelectedLogs(); 
                }

                // select all
                if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A)) {
                    selection.startRow = 0; 
                    selection.endRow = (int)filteredIndices.size() - 1; 
                    selection.isActive = true; 
                    selection.isCharMode = true; 
                    selection.startCol = 0; 
                    selection.endCol = std::numeric_limits<int>::max(); 
                } 

                // clear selection logic 
                // if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
                //     selection.clear();
                // }
            }
            ImGui::EndChild(); 

        }
        ImGui::End(); 
    }
}

void ConsoleUI::drawLogs() {
    const auto& logs = ConsoleEngine::getLogs(); 
    bool isNewLog = (logs.size() > lastLogSize); 
    lastLogSize = logs.size(); 
    
    updateSearchAndScroll(logs); 
    filteredIndices.clear(); 
    filteredIndices.reserve(logs.size()); 

    const LogEntry* lastLog = nullptr; 

    for (int i = 0; i < logs.size(); ++i) {    
        const auto& log = logs[i];
        
        if (log.level == LogLevel::ERROR && !togStates.isShowError) continue; 
        else if (log.level == LogLevel::WARNING && !togStates.isShowWarning) continue; 
        else if (log.level == LogLevel::INFO && !togStates.isShowInfo) continue; 
        else if (log.category == LogCategory::UI && !togStates.isShowUI) continue; 
        else if (log.category == LogCategory::ASSETS && !togStates.isShowAssets) continue; 
        else if (log.category == LogCategory::SHADER && !togStates.isShowShader) continue; 
        else if (log.category == LogCategory::OTHER && !togStates.isShowOther) continue;  
        else if (log.category == LogCategory::SYSTEM && !togStates.isShowSystem) continue; 
        
        // apply the collapse log logic
        if (togStates.isCollapsedLogs && lastLog) {
            bool isDuplicate = ( log.msg == lastLog->msg && 
                                 log.level == lastLog->level &&
                                 log.src == lastLog->src &&
                                 log.additional == lastLog->additional );
            if (isDuplicate) continue;
        }
        filteredIndices.push_back(i);
        lastLog = &log; 
    }

    if (isNewLog && togStates.isAutoScroll) {
        ImGui::SetScrollHereY(1.0f); 
    }

    if (TextSelector::Begin("##LogList", (int)filteredIndices.size(), selection)) {
        
        if (isNewLog && togStates.isAutoScroll) {
            ImGui::SetScrollHereY(1.0f); 
        }
        
        ImGuiListClipper clipper;
        clipper.Begin(filteredIndices.size(), TextSelector::GetLineHeight());

        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                int unfilteredIdx = filteredIndices[i];
                const auto& log = logs[unfilteredIdx];
                int repeatCount = getCollapseCount(logs, unfilteredIdx);
                
                std::string fullText = formatLogString(log);
                if (repeatCount > 0) fullText += " (" + std::to_string(repeatCount + 1) + ")";

                TextSelector::Text(fullText, i, [&]() {
                    LogStyle style = getLogStyle(log);
                    
                    ImGui::TextColored(style.color, "%s", style.prefix.c_str());
                    ImGui::SameLine(0, 0);
                    
                    if (log.additional.empty()) {
                        ImGui::TextUnformatted(log.msg.c_str());
                    } else {
                        ImGui::Text("%s %s", log.msg.c_str(), log.additional.c_str());
                    }

                    if (repeatCount > 0) {
                        ImGui::SameLine();
                        ImGui::TextDisabled("(%d)", repeatCount + 1);
                    }
                });
            }
        }
        TextSelector::End();
    }

}

const void ConsoleUI::drawMenuBar() {      
    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Clear")) {
                ConsoleEngine::executeBtnAction(ConsoleActions::CLEAR); 
            } 
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            ImGui::MenuItem("Auto-Scroll", nullptr, &togStates.isAutoScroll); 
            ImGui::MenuItem("Collapse Logs", nullptr, &togStates.isCollapsedLogs);
            ImGui::PopItemFlag(); 
            
            ImGui::MenuItem("Copy Logs"); 
            ImGui::MenuItem("Open Log History"); 
            ImGui::EndMenu(); 
        } 

        if (ImGui::BeginMenu("Filters")) {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 

            if (ImGui::MenuItem("Show Errors", nullptr, &togStates.isShowError))
                searcher.setDirty(true);

            if (ImGui::MenuItem("Show Warning", nullptr, &togStates.isShowWarning))
                searcher.setDirty(true);

            if (ImGui::MenuItem("Show Info", nullptr, &togStates.isShowInfo))
                searcher.setDirty(true); 

            ImGui::PopItemFlag(); 
            // Todo: add source filter to filter out shader errors, system errors, errors loading textures/objs, etc. 
            if (ImGui::BeginMenu("Show Sources")) {
                ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
                ImGui::MenuItem("Shader", nullptr, &togStates.isShowShader); 
                ImGui::MenuItem("System", nullptr, &togStates.isShowSystem); 
                ImGui::MenuItem("Assets", nullptr, &togStates.isShowAssets);
                ImGui::MenuItem("UI", nullptr, &togStates.isShowUI); 
                ImGui::MenuItem("Other", nullptr, &togStates.isShowOther);
                ImGui::PopItemFlag(); 
                ImGui::EndMenu(); 
            }
            ImGui::EndMenu(); 
        }

        // Used to test console UI 
        if (ImGui::BeginMenu("Spawn New Log")) {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            if (ImGui::MenuItem("Spawn 1 Error Log")) {
                Logger::addLog(LogLevel::ERROR, "Console Menu", "This is an error test", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 1 Warning Log")) {
                Logger::addLog(LogLevel::WARNING, "Console Menu", "This is a test warning", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 1 Info Log")) {
                Logger::addLog(LogLevel::INFO, "Console Menu", "This is a test", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 10 Info Logs")) {
                for (int i = 0; i < 10; i ++)
                    Logger::addLog(LogLevel::INFO, "Console Menu", "This is a test", "Additional"); 
            }
            ImGui::PopItemFlag(); 
            ImGui::EndMenu(); 
        }

        if (ImGui::BeginMenu("Find")) {
            if(searcher.drawSearchUI()) {

            }

            // ImGui::PopItemFlag(); 
            ImGui::EndMenu(); 
        }
        // Logger::addLog(LogLevel::INFO, "", "Drawing", -1); 
        ImGui::EndMenuBar(); 
    }
}

// updates search results from find and autoscroll if btn is active 
void ConsoleUI::updateSearchAndScroll(const std::deque<LogEntry> &logs) {
    if (searcher.GetisDirty() || (searcher.hasQuery() && logs.size() > lastLogSize)) {
        
        // used to avoid collapsed logs in search 
        const LogEntry* lastLog = nullptr; 
        searcher.updateMatches(logs, [&](const LogEntry &log) -> std::string {
            bool isDuplicate = false; 
            if(togStates.isCollapsedLogs && lastLog) {
                if (log.msg == lastLog ->msg && log.level == lastLog->level && 
                    log.src == lastLog->src && log.additional == lastLog->additional) {
                    
                        isDuplicate = true;
            }
        }

        lastLog = &log; 
        if(isDuplicate) return ""; 

        // avoid filtered logs in search
            if (log.level == LogLevel::ERROR   && !togStates.isShowError)   return "";
            if (log.level == LogLevel::WARNING && !togStates.isShowWarning) return "";
            if (log.level == LogLevel::INFO    && !togStates.isShowInfo)    return "";
            if (!togStates.isShowUI) return ""; 
            if (!togStates.isShowAssets) return ""; 
            if (!togStates.isShowShader) return ""; 
            if (!togStates.isShowOther) return "";  
            if (!togStates.isShowSystem) return ""; 
            

            return ConsoleUI::formatLogString(log); 
        });
    }

    if (logs.size() > lastLogSize) {
        lastLogSize = logs.size();
    }
}

int ConsoleUI::getCollapseCount(const std::deque<LogEntry> &logs, int currIdx) {

    if (!togStates.isCollapsedLogs) return 0;

    int count = 0;
    int nextIdx = currIdx + 1;
    
    while (nextIdx < logs.size()) {
        const auto& current = logs[currIdx];
        const auto& next = logs[nextIdx];

        bool isEqual = (current.msg == next.msg && 
                        current.level == next.level && 
                        current.src == next.src && 
                        current.additional == next.additional);

        if (isEqual) {
            count++;
            nextIdx++;
        } else {
            break;
        }
    }
    return count;
}

ConsoleUI::LogStyle ConsoleUI::getLogStyle(const LogEntry& log) {
    LogStyle style; 
    switch (log.level) {
        case LogLevel::CRITICAL: style.prefix = "[CRITICAL"; style.color = LOG_COLORS[(int)log.level]; break; 
        case LogLevel::ERROR: style.prefix = "[ERROR"; style.color = LOG_COLORS[(int)log.level]; break; 
        case LogLevel::WARNING: style.prefix = "[WARNING"; style.color = LOG_COLORS[(int)log.level]; break; 
        case LogLevel::INFO: style.prefix = "[INFO"; style.color = LOG_COLORS[(int)log.level]; break; 
    }

    if(!log.src.empty()) {
        style.prefix += ": " + log.src; 
    }

    style.prefix += "] "; 
    return style; 
}

std::string ConsoleUI::formatLogString(const LogEntry& log) {
    LogStyle style = getLogStyle(log); 

    std::string fullMsg = style.prefix + log.msg; 
    if(!log.additional.empty()) {
        fullMsg += " " + log.additional; 
    }

    return fullMsg; 
}

void ConsoleUI::copySelectedLogs() {
if (!selection.isActive) return; 

    const auto& logs = ConsoleEngine::getLogs();
    
    TextSelector::copyText(selection, (int)filteredIndices.size(), [&](int idx) -> std::string {
        int unfilteredIdx = filteredIndices[idx];
        const auto& log = logs[unfilteredIdx];
        
        std::string text = formatLogString(log);
        int repeatCount = getCollapseCount(logs, unfilteredIdx);
        if (repeatCount > 0) text += " (" + std::to_string(repeatCount + 1) + ")";
        
        return text;
    });

    Logger::addLog(LogLevel::INFO, "ConsoleUI", "Copied Text");
}
