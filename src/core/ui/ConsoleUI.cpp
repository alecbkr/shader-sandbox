#include "ConsoleUI.hpp"
#include "platform/Platform.hpp"
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
                    selection.startCol = 0; 
                    selection.endCol = std::numeric_limits<int>::max(); 
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

    // incase fonts prevent us from using the selection logic 
    if (isCurrentFontMonospace()) {
        drawLogsManualSelection(); 
    } else {
        Logger::addLog(LogLevel::WARNING,"ConsoleUI", "Font is not a monospace font rendering line selection"); 
        // ImGui optimization that prevents drawing each log per frame and only the ones that are visible on the window 
        float lineHeight = std::max(1.0f, ImGui::GetTextLineHeight()); 
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

        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
            selection.clear(); 
        }
    }
}

// TODO: Might make this a ui component so that we could use it anywhere else we may need it 
void ConsoleUI::drawLogsManualSelection () {
    const auto& logs = ConsoleEngine::getLogs(); 
    auto drawList = ImGui::GetWindowDrawList(); 

    float lineHeight = ImGui::GetTextLineHeight(); 
    float charWidth = ImGui::CalcTextSize("A").x; 
    ImVec2 screenPos = ImGui::GetCursorScreenPos(); 
    float scrollY = ImGui::GetScrollY(); 
    float contentWidth = ImGui::GetContentRegionAvail().x; 
    float totalHeight = (float)filteredIndices.size() * lineHeight; 

    ImGui::InvisibleButton("##LogCanvas", ImVec2(contentWidth, totalHeight)); 

    if (ImGui::IsWindowHovered() || ImGui::IsItemActive()) {
        ImVec2 mouse = ImGui::GetMousePos(); 
        float relX = mouse.x - screenPos.x; 
        float relY = mouse.y - screenPos.y + scrollY;

        int hoverRow = (int)(relY / lineHeight); 
        int hoverCol = (int)(relX / charWidth); 

        if (hoverRow < 0) hoverRow = 0; 
        if (hoverRow >= filteredIndices.size()) hoverRow = (int)filteredIndices.size() - 1;
        if (hoverCol < 0) hoverCol = 0; 

        // clicking logic 
        if (ImGui::IsMouseClicked(0)) {
            selection.startIdx = hoverRow; 
            selection.endIdx = hoverRow; 
            selection.startCol = hoverCol; 
            selection.endCol = hoverCol; 
            selection.active = true; 
            selection.isCharMode = true; 
        }

        // cursor dragging logic
        else if (ImGui::IsMouseDown(0) && selection.active) {
            selection.endIdx = hoverRow; 
            selection.endCol = hoverCol; 
            selection.isCharMode = true; 
        }
    }

    ImGuiListClipper clipper; 
    clipper.Begin(filteredIndices.size(), lineHeight); 

    int selStartRow = std::min(selection.startIdx, selection.endIdx); 
    int selEndRow = std::max(selection.startIdx, selection.endIdx); 

    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
            int unfilteredIdx = filteredIndices[i]; 
            const auto& log = logs[unfilteredIdx]; 
            std::string text = formatLogString(log); 
            int repeatCount = getCollapseCount(logs, unfilteredIdx);
            if (repeatCount > 0) {
                text += " (" + std::to_string(repeatCount + 1) + ")"; 
            }
            float lineY = screenPos.y + (i * lineHeight); 

            // draw selection highlighting 
            if (selection.active && i >= selStartRow && i <= selEndRow) {
                float hlStart = 0.0f; 
                float hlEnd = contentWidth; 

                if (selStartRow == selEndRow) {
                    int sCol = std::min(selection.startCol, selection.endCol); 
                    int eCol = std::max(selection.startCol, selection.endCol); 
                    hlStart = sCol * charWidth; 
                    hlEnd = (eCol + 1) * charWidth; 
                }

                // Top line of multi-selection
                else if (i == selStartRow) {
                    int sCol = (selection.startIdx < selection.endIdx) ? selection.startCol : selection.endCol; 
                    hlStart = sCol * charWidth; 
                }

                else if (i == selEndRow) {
                    int eCol = (selection.startIdx < selection.endIdx) ? selection.endCol : selection.startCol; 
                    hlEnd = (eCol + 1) * charWidth; 
                }

                drawList->AddRectFilled(
                    ImVec2(screenPos.x + hlStart, lineY), 
                    ImVec2(screenPos.x + hlEnd, lineY + lineHeight), 
                    IM_COL32(0, 120, 215, 128)
                ); 
            }

            // draw the text 
            ImGui::SetCursorScreenPos(ImVec2(screenPos.x, lineY));
            LogStyle style = getLogStyle(log);
            ImGui::TextColored(style.color, "%s", text.c_str());
        }
    }
    // Create a dummy item so that ImGui is able to detect how tall the content is 
    // ImGui::SetCursorScreenPos(screenPos);
    // ImGui::Dummy(ImVec2(contentWidth, filteredIndices.size() * lineHeight));
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

//  The selection logic for selecting indivual chars is dependent on this. 
bool ConsoleUI::isCurrentFontMonospace() {
    // use the widest and largest chars available  
    float widthW = ImGui::CalcTextSize("W").x; 
    float widthi = ImGui::CalcTextSize("i").x; 
    return std::abs(widthW - widthi) < 0.001f; 
}

void ConsoleUI::copyManualSelection() {
    if (!selection.active) return;
    const auto& logs = ConsoleEngine::getLogs();
    std::string clipText;

    int startRow = selection.startIdx;
    int endRow = selection.endIdx;
    int startCol = selection.startCol;
    int endCol = selection.endCol;

    // Standardize direction
    if (startRow > endRow) { std::swap(startRow, endRow); std::swap(startCol, endCol); }
    if (startRow == endRow && startCol > endCol) { std::swap(startCol, endCol); }

    for (int i = startRow; i <= endRow; ++i) {
        if (i < 0 || i >= filteredIndices.size()) continue;

        std::string line = formatLogString(logs[filteredIndices[i]]);
        int repeatCount = getCollapseCount(logs, filteredIndices[i]);
        if(repeatCount > 0) line += " (" + std::to_string(repeatCount+1) + ")";

        int subStart = 0;
        int subLen = line.length();

        if (i == startRow) {
            subStart = std::clamp(startCol, 0, (int)line.length());
            subLen -= subStart;
        }
        if (i == endRow) {
            int effEnd = std::clamp(endCol + 1, 0, (int)line.length());
            subLen = effEnd - subStart;
        }

        if (subLen > 0) clipText += line.substr(subStart, subLen);
        if (i != endRow) clipText += "\n";
    }

    if (!clipText.empty()) ImGui::SetClipboardText(clipText.c_str());
}