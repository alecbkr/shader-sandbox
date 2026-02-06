#include "ConsoleUI.hpp"
#include "platform/Platform.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include "platform/Platform.hpp"

bool ConsoleUI::initialized = false; 
size_t ConsoleUI::lastLogSize = 0;
float ConsoleUI::targetWidth = 0.0f;
float ConsoleUI::targetHeight = 0.0f;
ImVec2 ConsoleUI::windowPos = ImVec2(0, 0);
// int ConsoleUI::selectionStart = -1; 
// int ConsoleUI::selectionEnd = -1;
SearchText ConsoleUI::searcher; 
ConsoleToggles& ConsoleUI::togStates = ConsoleEngine::getToggles();
LogSelection ConsoleUI::selection; 
std::vector<int> ConsoleUI::filteredIndices; 

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
                    selection.startIdx = 0; 
                    selection.endIdx = (int)filteredIndices.size() - 1; 
                    selection.active = true; 
                } 

                // clear selection logic 
                if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
                    selection.clear();
                }
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

    // for (int i = 0; i < logs.size(); ++i) {
    //     int repeatCount = getCollapseCount(logs, i); 
    //     drawSingleLog(logs[i], i, repeatCount); 
    //     i += repeatCount; 
    // }

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

    // ImGui optimization that prevents drawing each log per frame and only the ones that are visible on the window 
    ImGuiListClipper clipper; 
    clipper.Begin(filteredIndices.size()); 

    while (clipper.Step()) {
        for (int filteredIdx = clipper.DisplayStart; filteredIdx < clipper.DisplayEnd; filteredIdx++) {
            int unfilteredIdx = filteredIndices[filteredIdx]; 
            const auto& log = logs[unfilteredIdx]; 
            int repeatCount = getCollapseCount(logs, unfilteredIdx); 
            drawSingleLog(log, unfilteredIdx, filteredIdx, repeatCount);
        }
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

// void ConsoleUI::drawSingleLog(const LogEntry& log, int idx, int repeatCount) {    
//     LogStyle style = getLogStyle(log);
//     ImGui::PushID(idx);
//     ImVec2 screenPos = ImGui::GetCursorScreenPos();

//     // logic for checking collapsed logs 
//     int matchIndexToCheck = idx; 
//     if (repeatCount > 0 && searcher.hasMatches()) {
//         const auto& activeMatch = searcher.getActiveMatch(); 
//         if(activeMatch.itemIdx > idx && activeMatch.itemIdx <= (idx + repeatCount)) {
//             matchIndexToCheck = activeMatch.itemIdx; 
//         }
//     }

//     // Draw the search highlight 
//     if (searcher.isItemActiveMatch(matchIndexToCheck)) {
//         if (searcher.checkAndClearScrollRequest()) {
//             ImGui::SetScrollHereY(0.5f);
//         }

//         const auto& match = searcher.getActiveMatch();
//         std::string fullText = formatLogString(log);

//         if (match.charIdx + match.length <= fullText.size()) {
//             std::string textBefore = fullText.substr(0, match.charIdx);
//             std::string textMatch = fullText.substr(match.charIdx, match.length);

//             float offsetX = ImGui::CalcTextSize(textBefore.c_str()).x;
//             float width = ImGui::CalcTextSize(textMatch.c_str()).x;

//             ImGui::GetWindowDrawList()->AddRectFilled(
//                 ImVec2(screenPos.x + offsetX, screenPos.y),
//                 ImVec2(screenPos.x + offsetX + width, screenPos.y + ImGui::GetTextLineHeight()),
//                 IM_COL32(200, 200, 200, 100)
//             );
//         }
//     }

//     ImGui::TextColored(style.color, "%s", style.prefix.c_str());
//     ImGui::SameLine(0, 0);
    
//     if (log.additional.empty()) {
//         ImGui::TextUnformatted(log.msg.c_str());
//     } else {
//         std::string fullMsg = log.msg + " " + log.additional;
//         ImGui::TextUnformatted(fullMsg.c_str());
//     }

//     // 6. Draw Collapse Counter
//     if (repeatCount > 0) {
//         ImGui::SameLine();
//         ImGui::TextDisabled("(%d)", repeatCount + 1);
//     }

//     ImGui::PopID();
// }

void ConsoleUI::drawSingleLog(const LogEntry& log, int unfilteredIdx, int filteredIdx, int repeatCount) {    
    LogStyle style = getLogStyle(log);
    ImGui::PushID(filteredIdx);
    ImVec2 screenPos = ImGui::GetCursorScreenPos();

    // selection logic 
    auto [selMin, selMax] = selection.getRange();
    bool isSelected = (selection.active && filteredIdx >= selMin && filteredIdx <= selMax);

    if (ImGui::Selectable("##row", isSelected, ImGuiSelectableFlags_SpanAvailWidth)) {
        if (ImGui::GetIO().KeyShift) {
            if (selection.startIdx == -1) {
                selection.startIdx = filteredIdx; 
            }
            selection.endIdx = filteredIdx; 
        } else {
            selection.startIdx = filteredIdx; 
            selection.endIdx = filteredIdx; 
            selection.active = true; 
        }
    }

    // cursor dragging logic 
    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (selection.active) selection.endIdx = filteredIdx; 
    }

    // render the ctx menu to copy the logs
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Copy")) {
            if (!isSelected) {
                selection.startIdx = filteredIdx; 
                selection.endIdx = filteredIdx; 
                selection.active = true; 
            }
            copySelectedLogs(); 
        }
        ImGui::EndPopup(); 
    }

    // search highlight logic 
    int matchIndxToCheck = unfilteredIdx; 

    if (repeatCount > 0 && searcher.hasMatches()) {
        const auto& activeMatch = searcher.getActiveMatch(); 
        if (activeMatch.itemIdx > unfilteredIdx && activeMatch.itemIdx <= (unfilteredIdx + repeatCount)) {
            matchIndxToCheck = activeMatch.itemIdx; 
        }
    }

    // matches the input string with strings in the source and highlights them if they match 
    if (searcher.isItemActiveMatch(matchIndxToCheck)) {
        if (searcher.checkAndClearScrollRequest()) {
            ImGui::SetScrollHereY(0.5f); 
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

    ImGui::SetCursorScreenPos(screenPos); 
    ImGui::TextColored(style.color, "%s", style.prefix.c_str());
    ImGui::SameLine(0, 0);
    
    if (log.additional.empty()) {
        ImGui::TextUnformatted(log.msg.c_str());
    } else {
        std::string fullMsg = log.msg + " " + log.additional;
        ImGui::TextUnformatted(fullMsg.c_str());
    }

    // 6. Draw Collapse Counter
    if (repeatCount > 0) {
        ImGui::SameLine();
        ImGui::TextDisabled("(%d)", repeatCount + 1);
    }

    ImGui::PopID();
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
    if (!selection.active) return; 

    auto [start, end] = selection.getRange(); 
    const auto& logs = ConsoleEngine::getLogs(); 
    std::string clipText; 
    clipText.reserve((end - start + 1) * 128); 

    for (int i = start; i <= end; ++i) {
        if (i < 0 || i >= filteredIndices.size()) continue;
        int unfilteredIdx = filteredIndices[i];
        clipText += formatLogString(logs[unfilteredIdx]) + '\n'; 
    }

    if (!clipText.empty()) {
        ImGui::SetClipboardText(clipText.c_str()); 
        Logger::addLog(LogLevel::INFO, "ConsoleUI", "Copied Text"); 
    }
}

// void ConsoleUI::copyLogsToClipboard() {
//     const auto& logs = ConsoleEngine::getLogs(); 
//     if(logs.empty()) return; 
//     size_t maxLogs = 128; 
//     size_t maxLineLength = 256; 
//     size_t startIdx = (logs.size() > maxLogs) ? (logs.size() - maxLogs) : 0;

//     std::string clipTxt; 
//     clipTxt.reserve(maxLogs * 256); 

//     for(size_t i = startIdx; i < logs.size(); ++i) {
//         std::string line = formatLogString(logs[i]); 

//         if (line.length() > 256) {
//             line = line.substr(0, 256) + "..."; 
//         }

//         clipTxt += line + "\n"; 
//     }

//     if (!clipTxt.empty()) {
//         ImGui::SetClipboardText(clipTxt.c_str()); 
//         Logger::addLog(LogLevel::INFO, "ConsoleUI", "Copied Text"); 
//     }
// }
