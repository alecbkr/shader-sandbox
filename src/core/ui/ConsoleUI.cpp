#include "ConsoleUI.hpp"
#include "platform/Platform.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cstdlib>
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
    lastLogSize = 0;
    initialized = false;
}

bool ConsoleUI::initialize(Logger* _loggerPtr) {
    if (initialized) {
        _loggerPtr->addLog(LogLevel::WARNING, "Console UI Initialization", "Console UI was already initialized.");
        return false;
    }
    engine = std::make_shared<ConsoleEngine>();

    if (!engine->initialize(_loggerPtr)) return false; 

    // had to register it here since the ui is responsible for copying the logs 
    engine->registerButton(ConsoleActions::COPY_LOGS, [this](){
        std::string fullLogText = engine->getFilteredLogText();
        if (!fullLogText.empty()) {
            ImGui::SetClipboardText(fullLogText.c_str());
        }
    });

    logSrc = _loggerPtr->getConsoleSinkPtr();
    loggerPtr = _loggerPtr;

    searcher.setSearchFlag(SearchUIFlags::ADVANCED);
    initialized = true;
    return true;
}

// change this to draw the entire componenet
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
        // ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_AlwaysHorizontalScrollbar; 
        ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
        drawLogs();
        ImGui::EndChild();
    }
    ImGui::End();
}

void ConsoleUI::drawLogs() {
    if (!logSrc || !engine) return; 

    const auto& logs = logSrc->getLogs();
    bool isScroll = false;

    updateSearchAndScroll(logs, isScroll);
    
    float wrapWidth = ImGui::GetWindowSize().x - ImGui::GetStyle().ScrollbarSize - 15.0f;
    wrapWidth = std::max(wrapWidth, 50.0f); 

    std::vector<DisplayLine> displayLines; 

    // get the filtered logs if filters are applied
    for (int i = 0; i < logs.size();) {
        if (!isLogFiltered(logs[i])) {
            int collapsedCount = getCollapseCount(logs, i);
            
            std::string rawStr = formatLogString(logs[i]); 
            auto wrappedLines = wrapLogText(rawStr, i, collapsedCount, wrapWidth); 
            displayLines.insert(displayLines.end(), wrappedLines.begin(), wrappedLines.end()); 

            i += collapsedCount + 1; 
        } else {
            i++; 
        }
    }

    if (TextSelector::Begin("ConsoleLogs", displayLines.size(), selectionCtx, selectionLayout)) {
        for (int row = 0; row < displayLines.size(); ++row) {
            drawSingleLog(row, displayLines[row], logs[displayLines[row].originalLogIdx], isScroll); 
        }
        TextSelector::End(); 
    }

    
    if (selectionCtx.isActive && ImGui::IsWindowFocused() && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
        TextSelector::copyText(selectionCtx, displayLines.size(), [&](int row, bool& isWrap) -> std::string {
            if (row >= 0 && row < displayLines.size()) {
                isWrap = displayLines[row].isWrap;
                std::string text = displayLines[row].text;
                if (displayLines[row].collapsedCount > 0) {
                    text += " (" + std::to_string(displayLines[row].collapsedCount + 1) + ")";
                }
                return text;
            }
            
            isWrap = false;
            return "";
        });
    }

    if (isScroll && engine->getToggles().isAutoScroll) {
        ImGui::SetScrollHereY(1.0f);
    }
}

 void ConsoleUI::drawMenuBar() {
    if(ImGui::BeginMenuBar()) {auto& togStates = engine->getToggles();

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Clear")) {
                engine->executeBtnAction(ConsoleActions::CLEAR);
                selectionCtx.clear(); 
            }
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);

            if (ImGui::MenuItem("Auto-Scroll", nullptr, &togStates.isAutoScroll)) {}
            if (ImGui::MenuItem("Collapse Logs", nullptr, &togStates.isCollapsedLogs)) {} 
            
            if (ImGui::MenuItem("Copy Logs")) {
                engine->executeBtnAction(ConsoleActions::COPY_LOGS); 
            }
            if (ImGui::MenuItem("Open Log History")) {
                // std::string p = loggerPtr->getLogPath().string();      
                engine->executeBtnAction(ConsoleActions::OPEN_LOG_HISTORY);           
                // openLogFile(p);
            } 
            ImGui::PopItemFlag();
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
            if (ImGui::MenuItem("Spawn 1 Overflow Log Test")) {
                loggerPtr->addLog(LogLevel::INFO, "Console Menu", "This is a test to test the overflow of the console. This is a test to test the overflow of the console. This is a test to test the overflow of the console. This is a test to test the overflow of the console. This is a test to test the overflow of the console. This is a test to test the overflow of the console", "Additional"); 
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
                if (log.msg == lastLog->msg && log.level == lastLog->level &&
                    log.src == lastLog->src && log.additional == lastLog->additional) {
                    isDuplicate = true;
            }
        }

        lastLog = &log;
        if(isDuplicate) return "";
        if (isLogFiltered(log)) return ""; 
        
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

void ConsoleUI::drawSingleLog(int rowIdx, const DisplayLine& lineData, const LogEntry& originalLog, bool& isScroll) {    
    LogStyle style = getLogStyle(originalLog);
    std::string rawText = lineData.text; 

    if (lineData.collapsedCount > 0) {
        rawText += " (" + std::to_string(lineData.collapsedCount + 1) + ")"; 
    }

    TextSelector::Text(rawText, [&]() {
        ImGui::PushID(rowIdx); 
        ImVec2 screenPos = ImGui::GetCursorScreenPos(); 

        int logIdx = lineData.originalLogIdx; 
        int idxToCheck = logIdx;
        if (lineData.collapsedCount > 0 && searcher.hasMatches()) {
            const auto& activeMatch = searcher.getActiveMatch(); 
            if(activeMatch.itemIdx > logIdx && activeMatch.itemIdx <= (logIdx + lineData.collapsedCount)) {
                idxToCheck = activeMatch.itemIdx; 
            }
        } 

        // Draw Search Highlight if its active
        if (searcher.isItemActiveMatch(idxToCheck)) {
            if (searcher.checkAndClearScrollRequest()) {
                ImGui::SetScrollHereY(0.5f);
                isScroll = false; 
            }


            // only highlight the portion of the matched string that falls on the curr wrapped line 
            const auto& match = searcher.getActiveMatch();

            int lineStart = lineData.charOffset;
            int lineEnd = lineStart + lineData.text.length();
            int matchStart = match.charIdx;
            int matchEnd = match.charIdx + match.length;

            if (matchEnd > lineStart && matchStart < lineEnd) {
                int localStart = std::max(0, matchStart - lineStart);
                int localEnd = std::min((int)lineData.text.length(), matchEnd - lineStart);
                
                std::string textBefore = lineData.text.substr(0, localStart);
                std::string textMatch = lineData.text.substr(localStart, localEnd - localStart);

                float offsetX = ImGui::CalcTextSize(textBefore.c_str()).x;
                float width = ImGui::CalcTextSize(textMatch.c_str()).x;

                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(screenPos.x + offsetX, screenPos.y),
                    ImVec2(screenPos.x + offsetX + width, screenPos.y + ImGui::GetTextLineHeight()),
                    IM_COL32(200, 200, 200, 100)
                );
            }
        }

        // ImGui::TextColored(style.color, "%s", style.prefix.c_str());
        // ImGui::SameLine(0, 0);        

        // only print the log info if it's the first line of the wrapped text 
        if (lineData.charOffset == 0 && lineData.text.length() >= style.prefix.length()) {
            ImGui::TextColored(style.color, "%s", style.prefix.c_str());
            ImGui::SameLine(0, 0);        
            
            std::string msgWithoutPrefix = lineData.text.substr(style.prefix.length());
            ImGui::TextUnformatted(msgWithoutPrefix.c_str());
        } else {
            ImGui::TextUnformatted(lineData.text.c_str());
        }

        if (lineData.collapsedCount > 0) {
            ImGui::SameLine();
            ImGui::TextDisabled("(%d)", lineData.collapsedCount + 1);
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

std::vector<ConsoleUI::DisplayLine> ConsoleUI::wrapLogText(const std::string& fullText, int logIndex, int collapseCount, float maxWidth) {
    std::vector<DisplayLine> result;
    if (fullText.empty()) {
        result.push_back({logIndex, "", false, collapseCount, 0});
        return result;
    }

    const char* textBegin = fullText.c_str();
    const char* textEnd = textBegin + fullText.length();
    const char* const basePtr = textBegin; 
    ImFont* font = ImGui::GetFont();

    while (textBegin < textEnd) {
        int currOffset = (int)(textBegin - basePtr); 
        const char* wrapPos = font->CalcWordWrapPositionA(1.0f, textBegin, textEnd, maxWidth);

        if (!wrapPos) wrapPos = textEnd; 

        if (wrapPos == textBegin) wrapPos++; 
        

        std::string lineText(textBegin, wrapPos);
        textBegin = wrapPos;

        if (!lineText.empty() && lineText.back() == '\n') {
            lineText.pop_back(); 
        }

        while (textBegin < textEnd && (*textBegin == ' ' || *textBegin == '\n')) {
            textBegin++;
        }

        bool isSoftWrap = (textBegin < textEnd);
        
        int displayCollapse = isSoftWrap ? 0 : collapseCount;

        result.push_back({logIndex, lineText, isSoftWrap, displayCollapse, currOffset});
    }
    return result;
}

bool ConsoleUI::isLogFiltered(const LogEntry& log) {
    if (!engine) return false;
    return engine->isLogFiltered(log);
}