#include "ConsoleUI.hpp"
#include "string"
#include "iostream"
#include <iostream>
#include "platform/Platform.hpp"

bool ConsoleUI::initialized = false; 
size_t ConsoleUI::lastLogSize = 0;
float ConsoleUI::targetWidth = 0.0f;
float ConsoleUI::targetHeight = 0.0f;
ImVec2 ConsoleUI::windowPos = ImVec2(0, 0);
int ConsoleUI::selectionStart = -1; 
int ConsoleUI::selectionEnd = -1;
SearchText ConsoleUI::searcher; 

ConsoleBtns ConsoleUI::btns = {
    .isAutoScroll = true, 
    .isCollapsedLogs = false, 
    .isShowErrors = true, 
    .isShowWarning = true, 
    .isShowInfo = true, 
    .isShowShader = true, 
    .isShowSystem = true, 
    .isShowAssets = true
}; 

std::vector<std::string> ConsoleUI::history{};

bool ConsoleUI::initialize() {
    // TODO: maybe create a file that implements all these callbacks so this function isn't as messy 
    ConsoleEngine::registerToggle(ConsoleEngine::Cmd::AUTO_SCROLL, 
        [&](bool state){btns.isAutoScroll = state;});
    ConsoleEngine::registerToggle(ConsoleEngine::Cmd::COLLAPSE_LOGS, 
        [&](bool state) {btns.isCollapsedLogs = state; });


    searcher.setSearchFlag(SearchUIFlags::ADVANCED); 

    initialized = true;

    // for (int i = 0; i < 200; ++i) {
    //     Logger::addLog(LogLevel::INFO, "Testing", "This is a test"); 
    // }
    // Logger::addLog(LogLevel::INFO, "Testing", "This is a test"); 


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
            ImGui::EndChild(); 
        }
    }
    ImGui::End(); 
}

void ConsoleUI::drawLogs() {
    const auto& logs = ConsoleEngine::getLogs(); 
    bool isScroll = false; 
    updateSearchAndScroll(logs, isScroll); 

    for (int i = 0; i < logs.size(); ++i) {
        int repeatCount = getCollapseCount(logs, i); 
        drawSingleLog(logs[i], i, repeatCount, isScroll); 
        i += repeatCount; 
    }

    if (isScroll) {
        ImGui::SetScrollHereY(1.0f); 
    }

}

const void ConsoleUI::drawMenuBar() {         
    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Clear")) {
                ConsoleEngine::executeBtnAction("clear"); 
            } 
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            ImGui::MenuItem("Auto-Scroll", nullptr, &btns.isAutoScroll); 
            ImGui::MenuItem("Collapse Logs", nullptr, &btns.isCollapsedLogs);
            ImGui::PopItemFlag(); 
            
            ImGui::MenuItem("Copy Logs"); 
            ImGui::MenuItem("Open Log History"); 
            ImGui::EndMenu(); 
        } 

        if (ImGui::BeginMenu("Filters")) {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            ImGui::MenuItem("Show Errors", nullptr, &btns.isShowErrors);
            ImGui::MenuItem("Show Warning", nullptr, &btns.isShowWarning);
            ImGui::MenuItem("Show Info", nullptr, &btns.isShowInfo); 
            ImGui::PopItemFlag(); 
            // Todo: add source filter to filter out shader errors, system errors, errors loading textures/objs, etc. 
            if (ImGui::BeginMenu("Show Sources")) {
                ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
                ImGui::MenuItem("Shader", nullptr, &btns.isShowShader); 
                ImGui::MenuItem("System", nullptr, &btns.isShowSystem); 
                ImGui::MenuItem("Assets", nullptr, &btns.isShowAssets);
                ImGui::PopItemFlag(); 
                ImGui::EndMenu(); 
            }
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
void ConsoleUI::updateSearchAndScroll(const std::deque<LogEntry> &logs, bool& isScroll) {
    if (searcher.GetisDirty() || (searcher.hasQuery() && logs.size() > lastLogSize)) {
        searcher.updateMatches(logs, [](const LogEntry &log){ 
            return ConsoleUI::formatLogString(log); 
        });
    }

    if (logs.size() > lastLogSize) {
        lastLogSize = logs.size();
        if (selectionStart == -1 && btns.isAutoScroll && !searcher.hasQuery()) {
            isScroll = true;
        }
    }
}

int ConsoleUI::getCollapseCount(const std::deque<LogEntry> &logs, int currIdx) {
    if (!btns.isCollapsedLogs) return 0;

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
    ImGui::PushID(idx);
    ImVec2 screenPos = ImGui::GetCursorScreenPos();

    // Draw the search highlight 
    if (searcher.isItemActiveMatch(idx)) {
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

void ConsoleUI::copyLogsToClipboard() {
    const auto& logs = ConsoleEngine::getLogs(); 
    if(logs.empty()) return; 
    size_t maxLogs = 128; 
    size_t maxLineLength = 256; 
    size_t startIdx = (logs.size() > maxLogs) ? (logs.size() - maxLogs) : 0;

    std::string clipTxt; 
    clipTxt.reserve(maxLogs * 256); 

    for(size_t i = startIdx; i < logs.size(); ++i) {
        std::string line = formatLogString(logs[i]); 

        if (line.length() > 256) {
            line = line.substr(0, 256) + "..."; 
        }

        clipTxt += line + "\n"; 
    }

    if (!clipTxt.empty()) {
        ImGui::SetClipboardText(clipTxt.c_str()); 
        Logger::addLog(LogLevel::INFO, "ConsoleUI", "Copied Text"); 
    }
}
