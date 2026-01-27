#include "ConsoleUI.hpp"
#include "string"
#include "iostream"

bool ConsoleUI::initialized = false; 
size_t ConsoleUI::lastLogSize = 0;
int ConsoleUI::selectionStart = -1; 
int ConsoleUI::selectionEnd = -1;

ConsoleBtns ConsoleUI::btns = {
    false, 
    false, 
    true, 
    true, 
    true, 
    true, 
    true, 
    true
}; 

std::vector<std::string> ConsoleUI::history{};

bool ConsoleUI::initialize() {
    // TODO: maybe create a file that implements all these callbacks so this function isn't as messy 
    ConsoleEngine::registerToggle(ConsoleEngine::Cmd::AUTO_SCROLL, 
        [&](bool state){btns.isAutoScroll = state;});

    initialized = true;
    return true;
}

// change this to draw the entire componenet 
const void ConsoleUI::render() {
    if (!initialized) return;
     ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    
    if(ImGui::Begin("Console", nullptr, ImGuiWindowFlags_MenuBar)) {  
        ConsoleUI::drawMenuBar();

        bool isWinFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); 

        if (isWinFocused && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
            Logger::addLog(LogLevel::INFO, "ConsoleTest", "Hit Ctrl + C"); 
            copyLogsToClipboard(); 
        }

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
            ConsoleUI::selectionStart = -1; 
            ConsoleUI::selectionEnd = -1; 
        }

        {
            // std::string test_buffer = "Hello \n World \n Testing \n ..."; 
            ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
            ConsoleUI::drawLogs(); 
            // ImGui::InputTextMultiline("##source", test_buffer.data(), test_buffer.size() + 1, ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_ReadOnly);
            ImGui::EndChild(); 
        }
    }
    ImGui::End(); 
}

// TODO: allow for users to select multiple text boxes to copy 
void ConsoleUI::drawLogs(){
    const auto& logs = ConsoleEngine::getLogs(); 

    bool isScroll = false; 
    if (logs.size() > lastLogSize) {
        lastLogSize = logs.size(); 

        // only scroll when user is not dragging to copy text 
        if (selectionStart == -1 && btns.isAutoScroll) {
            isScroll = true; 
        }
    }

    static bool isCursorDragging = false; 

    for(int i = 0; i < logs.size(); i ++) {
        auto log = logs[i]; 
        LogStyle style = getLogStyle(log); 
        bool isSelected = false; 
        
        if(selectionStart != -1 && selectionEnd != -1) {
            int startIdx = (selectionStart < selectionEnd) ? selectionStart : selectionEnd; 
            int endIdx = (selectionStart > selectionEnd) ? selectionStart : selectionEnd; 
            isSelected = (i >= startIdx && i <= endIdx); 
        }

        ImGui::PushID(i); 
        ImVec2 lastCursorPos = ImGui::GetCursorPos(); 
        // ImGui::Selectable("##logline", isSelected, ImGuiSelectableFlags_SpanAllColumns);
        ImGui::Selectable("##logline", isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selectionStart = i; 
            selectionEnd = i; 
            isCursorDragging = true; 
        }

        // handles dragging logic for when users drag their cursor to select multiple lines from the console 
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && isCursorDragging) {
            selectionEnd = i; 
        }

        ImGui::SetCursorPos(lastCursorPos); 
        ImGui::TextColored(style.color, "%s", style.prefix.c_str());
        ImGui::SameLine(0,0); 
        
        if(log.additional.empty()) {
            ImGui::TextUnformatted(log.msg.c_str()); 
        } else {
            std::string fullMsg = log.msg + " " + log.additional; 
            ImGui::TextUnformatted(fullMsg.c_str()); 
        }

        ImGui::PopID(); 
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        isCursorDragging = false; 
    }

    if(isScroll) {
        ImGui::SetScrollHereY(1.0f); 
    }
}

const void ConsoleUI::drawMenuBar() {         
    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Clear", "Ctrl + l")) {
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
            ImGui::MenuItem("Show Errors", nullptr, btns.isShowErrors);
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

            ImGui::EndMenu(); 
        }
        // Logger::addLog(LogLevel::INFO, "", "Drawing", -1); 
        ImGui::EndMenuBar(); 
    }
}

void ConsoleUI::copyLogsToClipboard() {
    // nothing was selected in the console
    if (selectionStart == -1 || selectionEnd == -1) {
        Logger::addLog(LogLevel::INFO, "Console Test", "Nothing was selected"); 
        return; 
    }  

    // normalize between the start and end 
    int startIdx = (selectionStart < selectionEnd) ? selectionStart : selectionEnd; 
    int endIdx = (selectionStart > selectionEnd) ? selectionStart : selectionEnd; 

    std::string clipText; 
    auto& logs = ConsoleEngine::getLogs(); 

    for (int i = startIdx; i <= endIdx; ++i) {
        if(i >= 0 && i < logs.size()) {
            clipText += formatLogString(logs[i]) + "\n"; 

        }
    }

    // TODO: Maybe change this based on if we keep copy from the menu or not 
    if(!clipText.empty()) {
        ImGui::SetClipboardText(clipText.c_str()); 
    }
}

const void ConsoleUI::executeCommand() {
    // if(!engine) {
    //     return; 
    // }

    // std::string command(inputBuf); 
    // Logger::addLog(LogLevel::INFO, ">", command, ); 
    // engine->processInput(command); 
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

