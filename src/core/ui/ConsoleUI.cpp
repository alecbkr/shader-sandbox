#include "ConsoleUI.hpp"
#include "platform/Platform.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include "platform/Platform.hpp"
#include "../logging/Logger.hpp"

// Lookup table for text colors for each log level
const ImVec4 LOG_COLORS[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f),         // Critical (Deep Red) 
    ImVec4(1.0f, 0.4f, 0.4f, 1.0f),         // Error (lighter red)
    ImVec4(1.0f, 0.1f, 0.5f, 1.0f),         // Warning (Magenta)
    ImVec4(0.4f, 1.0f, 0.4f, 1.0f)          // Info (Light Green)
                                            // Anomaly (light gray)
}; 

ConsoleUI::ConsoleUI() {
    targetWidth = 0.0f;
    targetHeight = 0.0f;
    windowPos = ImVec2(0, 0);
    engine = nullptr;
    logSrc = nullptr;
    // isAutoScroll = false;
    lastLogSize = 0;
    initialized = false;
    
}

bool ConsoleUI::initialize(Logger* _loggerPtr) {
    if (initialized) {
        _loggerPtr->addLog(LogLevel::WARNING, "Console UI Initialization", "Console UI was already initialized.");
        return false;
    }
    engine = std::make_shared<ConsoleEngine>();
    logSrc = _loggerPtr->getConsoleSinkPtr();
    loggerPtr = _loggerPtr; 
    searcher.setSearchFlag(SearchUIFlags::ADVANCED);
    
    initialized = true;
    return true;
}
 
void ConsoleUI::render() {
    if (!initialized) return;
    float menuBarHeight = ImGui::GetFrameHeight();
    
    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    targetWidth = (float)displayWidth * 0.4f;
    targetHeight = (float)displayHeight * 0.3f;
    
    int editorOffsetY = displayHeight * 0.7f;
    
    windowPos.x = 0;
    windowPos.y = editorOffsetY + menuBarHeight;

    ImGui::SetNextWindowSize(ImVec2(targetWidth, targetHeight + 1), ImGuiCond_Always);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

    if (ImGui::Begin("Console", nullptr, flags)) {  
        drawMenuBar();

        ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
        drawLogs(); 
        ImGui::EndChild(); 
    }
    ImGui::End(); 
}

void ConsoleUI::drawLogs() {
    if (!logSrc || !engine) return; 

    struct FilteredLog {
        int idx; 
        int collapsedCount; 
    }; 

    const auto& logs = logSrc->getLogs(); 
    bool isScroll = false;

    updateSearchAndScroll(logs, isScroll);
    std::vector <FilteredLog> filteredLogs; 

    // get the filtered logs if filters are applied
    for (int i = 0; i < logs.size();) {
        if (!isLogFiltered(logs[i])) {
            int collapsedCount = getCollapseCount(logs, i);
            filteredLogs.push_back({i, collapsedCount});          
            i += collapsedCount + 1; 
        } else {
            i++; 
        }
    }

    if (TextSelector::Begin("ConsoleLogs", filteredLogs.size(), selectionCtx, selectionLayout)) {
        for (int row = 0; row < filteredLogs.size(); ++row) {
            const auto& filteredLog = filteredLogs[row]; 
            drawSingleLog(logs[filteredLog.idx], filteredLog.idx, filteredLog.collapsedCount, isScroll); 
        }
        TextSelector::End(); 
    }

    
    if (selectionCtx.isActive && ImGui::IsWindowFocused() && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
        TextSelector::copyText(selectionCtx, filteredLogs.size(), [&](int row) {
            if (row >= 0 && row < filteredLogs.size()) {
                const LogEntry& log = logs[filteredLogs[row].idx];
                std::string text = formatLogString(log);
                if (filteredLogs[row].collapsedCount > 0) {
                    text += " (" + std::to_string(filteredLogs[row].collapsedCount + 1) + ")";
                }
                return text;
            }
            return std::string("");
        }); 
    }

    if (isScroll && engine->getToggles().isAutoScroll) {
        ImGui::SetScrollHereY(1.0f); 
    }

}

void ConsoleUI::drawMenuBar() {      
    if (ImGui::BeginMenuBar()) {
        auto& togStates = engine->getToggles();

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Clear")) {
                engine->executeBtnAction(ConsoleActions::CLEAR); 
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

            if (ImGui::MenuItem("Show Errors", nullptr, &togStates.isShowError)) searcher.setDirty(true);
            if (ImGui::MenuItem("Show Warning", nullptr, &togStates.isShowWarning)) searcher.setDirty(true);
            if (ImGui::MenuItem("Show Info", nullptr, &togStates.isShowInfo)) searcher.setDirty(true); 

            ImGui::PopItemFlag(); 
            
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

        if (ImGui::BeginMenu("Spawn New Log")) {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            if (ImGui::MenuItem("Spawn 1 Error Log")) {
                loggerPtr->addLog(LogLevel::LOG_ERROR, "Console Menu", "This is an error test", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 1 Warning Log")) {
                loggerPtr->addLog(LogLevel::WARNING, "Console Menu", "This is a test warning", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 1 Info Log")) {
                loggerPtr->addLog(LogLevel::INFO, "Console Menu", "This is a test", "Additional"); 
            }
            if (ImGui::MenuItem("Spawn 10 Info Logs")) {
                for (int i = 0; i < 10; i++)
                    loggerPtr->addLog(LogLevel::INFO, "Console Menu", "This is a test", "Additional"); 
            }
            ImGui::PopItemFlag(); 
            ImGui::EndMenu(); 
        }

        if (ImGui::BeginMenu("Find")) {
            searcher.drawSearchUI();
            ImGui::EndMenu(); 
        }
        ImGui::EndMenuBar(); 
    }
}

// updates search results from find and autoscroll if btn is active 
void ConsoleUI::updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll) {
    auto& togStates = engine->getToggles(); 
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
            if (log.level == LogLevel::LOG_ERROR   && !togStates.isShowError)   return "";
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
        if (selectionStart == -1 && togStates.isAutoScroll && !searcher.hasQuery()) {
            isScroll = true;
        }
    }
}

int ConsoleUI::getCollapseCount(const std::deque<LogEntry> &logs, int currIdx) {
    auto& togStates = engine->getToggles(); 
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

void ConsoleUI::drawSingleLog(const LogEntry& log, int idx, int repeatCount, bool& isScroll) {
    LogStyle style = getLogStyle(log);
    std::string rawText = formatLogString(log); 

    if (repeatCount > 0) {
        rawText += " (" + std::to_string(repeatCount + 1) + ")"; 
    }

    TextSelector::Text(rawText, [&]() {
        ImGui::PushID(idx); 
        ImVec2 screenPos = ImGui::GetCursorScreenPos(); 

        int idxToCheck = idx;
        if (repeatCount > 0 && searcher.hasMatches()) {
            const auto& activeMatch = searcher.getActiveMatch(); 
            if(activeMatch.itemIdx > idx && activeMatch.itemIdx <= (idx + repeatCount)) {
                idxToCheck = activeMatch.itemIdx; 
            }
        } 

        // Draw Search Highlight if its active
        if (searcher.isItemActiveMatch(idxToCheck)) {
            if (searcher.checkAndClearScrollRequest()) {
                ImGui::SetScrollHereY(0.5f);
                isScroll = false; 
            }

            const auto& match = searcher.getActiveMatch();
            std::string fullText = formatLogString(log);

            if (match.charIdx + match.length <= fullText.size()) {
                std::string textBefore = fullText.substr(0, match.charIdx);
                std::string textMatch = fullText.substr(match.charIdx, match.length);

                float offsetX = ImGui::CalcTextSize(textBefore.c_str()).x;
                float width = ImGui::CalcTextSize(textMatch.c_str()).x;

                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(screenPos.x + offsetX, screenPos.y),
                    ImVec2(screenPos.x + offsetX + width, screenPos.y + ImGui::GetTextLineHeight()),
                    IM_COL32(200, 200, 200, 100)
                );
            }
        }

        // Draw the actual colored text using the custom widget 
        ImGui::TextColored(style.color, "%s", style.prefix.c_str());
        ImGui::SameLine(0, 0);
        
        if (log.additional.empty()) {
            ImGui::TextUnformatted(log.msg.c_str());
        } else {
            std::string fullMsg = log.msg + " " + log.additional;
            ImGui::TextUnformatted(fullMsg.c_str());
        }

        if (repeatCount > 0) {
            ImGui::SameLine();
            ImGui::TextDisabled("(%d)", repeatCount + 1);
        }

        ImGui::PopID();
    }); 
    

}

ConsoleUI::LogStyle ConsoleUI::getLogStyle(const LogEntry& log) {
    LogStyle style; 
    style.color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // default white for anomalies 
    switch (log.level) {
        case LogLevel::CRITICAL: style.prefix = "[CRITICAL"; style.color = LOG_COLORS[(int)log.level]; break; 
        case LogLevel::LOG_ERROR: style.prefix = "[ERROR"; style.color = LOG_COLORS[(int)log.level]; break; 
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

bool ConsoleUI::isLogFiltered(const LogEntry& log) {
    auto& togStates = engine->getToggles(); 

    // Message type
    if (log.level == LogLevel::LOG_ERROR && !togStates.isShowError) return true;

    // source types 
    if (log.category == LogCategory::UI && !togStates.isShowUI) return true; 
    if (log.category == LogCategory::ASSETS && !togStates.isShowAssets) return true; 
    if (log.category == LogCategory::SHADER && !togStates.isShowShader) return true; 
    if (log.category == LogCategory::OTHER && !togStates.isShowOther) return true;  
    if (log.category == LogCategory::SYSTEM && !togStates.isShowSystem) return true;

    return false; 
}