#include "ConsoleUI.hpp"

ConsoleUI::ConsoleUI(std::shared_ptr<ConsoleSink> consoleSink){
    engine = std::make_shared<ConsoleEngine>(); 
    logSrc = consoleSink; 
} 

ConsoleUI::~ConsoleUI(){}

const void ConsoleUI::render() {
    drawConsole(); 
}

const void ConsoleUI::drawConsole(){
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    
    if(ImGui::Begin("Console", nullptr, ImGuiWindowFlags_MenuBar)) {  
        ConsoleUI::drawMenuBar();
        {
            ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
            ConsoleUI::readLogs(); 
            ImGui::EndChild(); 
        }
    }
    ImGui::End(); 

}

void ConsoleUI::readLogs(){
    if (!logSrc) return; 

    const auto& logs = logSrc->getLogs(); 
    int lineNum = 0; 
    for(const auto& log:logs) {
        int idx = std::min((int)log.level, 3); // clamp the index to avoid out-of-bounds
        std::string alert; 

        switch (log.level) {
            case LogLevel::CRITICAL:  alert = "CRITICAL: ";  break; 
            case LogLevel::ERROR:     alert = "ERROR: ";     break; 
            case LogLevel::WARNING:   alert = "WARNING: ";   break; 
            case LogLevel::INFO:      alert = "INFO: ";      break; 
            default:                  alert = "ANOMALY: ";   break; 
        }

        // TODO: Find some way of making the text copyable 
        ImGui::PushStyleColor(ImGuiCol_Text, LOG_COLORS[idx]); 
        ImGui::TextUnformatted(alert.c_str()); 
        ImGui::SameLine(0.0f, 0.0f); 
        ImGui::PopStyleColor(); 
        ImGui::TextUnformatted(log.msg.c_str()); 
        
        lineNum++;
    }

    //     // TODO: add src if there is a src file 
    // }

    if (logs.size() > lastLogSize) {
        isAutoScroll = true; 
        lastLogSize = logs.size(); 
    }

    if(isAutoScroll) {
        ImGui::SetScrollHereY(1.0f); 
        isAutoScroll = false; 
    }
}

const void ConsoleUI::drawMenuBar() {

    // TODO: modify this behavior once we have some sort of way to store user preferences
    // View menu
    static bool isAutoScroll = false;
    static bool isCollapsedLogs = false; 
    // Filters menu
    static bool isShowErrors = true; 
    static bool isShowWarnings = true; 
    static bool isShowInfo = true; 
    // Filters/ShowSources menu 
    static bool isShowShader = true; 
    static bool isShowSystem = true; 
    static bool isShowAssets = true; 

         
    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Clear", "Ctrl + l"); 

            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            ImGui::MenuItem("Auto-Scroll", nullptr, &isAutoScroll); 
            ImGui::MenuItem("Collapse Logs", nullptr, &isCollapsedLogs);
            ImGui::PopItemFlag(); 
            
            ImGui::MenuItem("Copy Logs"); 
            ImGui::MenuItem("Open Log History"); 
            ImGui::EndMenu(); 
        } 

        if (ImGui::BeginMenu("Filters")) {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
            ImGui::MenuItem("Show Errors", nullptr, &isShowErrors);
            ImGui::MenuItem("Show Warning", nullptr, &isShowWarnings);
            ImGui::MenuItem("Show Info", nullptr, &isShowInfo); 
            ImGui::PopItemFlag(); 
            // Todo: add source filter to filter out shader errors, system errors, errors loading textures/objs, etc. 
            if (ImGui::BeginMenu("Show Sources")) {
                ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); 
                ImGui::MenuItem("Shader", nullptr, &isShowShader); 
                ImGui::MenuItem("System", nullptr, &isShowSystem); 
                ImGui::MenuItem("Assets", nullptr, &isShowAssets);
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

const void ConsoleUI::executeCommand() {
    if(!engine) {
        return; 
    }

    std::string command(inputBuf); 
    Logger::addLog(LogLevel::INFO, ">", command, -1); 
    engine->processInput(command); 
}