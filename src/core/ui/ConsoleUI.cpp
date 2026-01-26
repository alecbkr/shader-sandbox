#include "ConsoleUI.hpp"
#include <algorithm>
#include <iostream>

bool ConsoleUI::initialized = false;
bool ConsoleUI::isAutoScroll = false;
size_t ConsoleUI::lastLogSize = 0;
float ConsoleUI::targetWidth = 0.0f;
float ConsoleUI::targetHeight = 0.0f;
ImVec2 ConsoleUI::windowPos = ImVec2(0, 0);

std::shared_ptr<ConsoleEngine> ConsoleUI::engine = nullptr;
std::shared_ptr<ConsoleSink> ConsoleUI::logSrc = nullptr;
std::vector<std::string> ConsoleUI::history{};

bool ConsoleUI::initialize(std::shared_ptr<ConsoleSink> consoleSink) {
    engine = std::make_shared<ConsoleEngine>();
    logSrc = consoleSink;

    initialized = true;
    return true;
}

const void ConsoleUI::render() {
    if (!initialized) return;
    drawConsole(); 
}

const void ConsoleUI::drawConsole(){
    float menuBarHeight = ImGui::GetFrameHeight();
    
    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    ConsoleUI::targetWidth = (float)displayWidth * 0.4f;
    ConsoleUI::targetHeight = (float)displayHeight * 0.3f;
    
    int editorOffsetY = displayHeight * 0.7f;
    
    ConsoleUI::windowPos.x = 0;
    ConsoleUI::windowPos.y = editorOffsetY + menuBarHeight;

    ImGui::SetNextWindowSize(ImVec2(targetWidth, targetHeight + 1), ImGuiCond_Once);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

    if(ImGui::Begin("Console", nullptr, flags)) {  
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

    for(const auto& log:logs) {
        int idx = std::min((int)log.level, 3); // clamp the index to avoid out-of-bounds
        std::string alert = ""; 
        switch (log.level) {
            case LogLevel::CRITICAL:  alert = "[CRITICAL: " + log.src + "] ";   break; 
            case LogLevel::ERROR:     alert += "[ERROR: " + log.src + "] "  ;   break; 
            case LogLevel::WARNING:   alert += "[WARNING: " + log.src + "] ";   break; 
            case LogLevel::INFO:      alert += "[INFO: " + log.src + "] ";      break; 
            default:                  alert += "[ANOMALY: " + log.src + "] ";   break; 
        }

        // TODO: Find some way of making the text copyable 
        ImGui::PushStyleColor(ImGuiCol_Text, LOG_COLORS[idx]); 
        ImGui::TextUnformatted(alert.c_str()); 
        ImGui::SameLine(0.0f, 0.0f); 
        ImGui::PopStyleColor(); 
        ImGui::TextUnformatted(log.msg.c_str());

        if (!log.additional.empty()) {
            std::string additional = "  " + log.additional; 
            ImGui::TextUnformatted(additional.c_str());  
        }
    }

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

    // std::string command(inputBuf); 
    // Logger::addLog(LogLevel::INFO, ">", command, ); 
    // engine->processInput(command); 
}