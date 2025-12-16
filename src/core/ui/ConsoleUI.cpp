#include "ConsoleUI.hpp"

ConsoleUI::ConsoleUI(){
    shbx::Logger::addLog(shbx::LogLevel::INFO, "", "Welcome to Shader Sandbox");
    shbx::Logger::addLog(shbx::LogLevel::CRITICAL, "", "Example Critical Error"); 
    shbx::Logger::addLog(shbx::LogLevel::ERROR, "", "Example Error"); 
    shbx::Logger::addLog(shbx::LogLevel::WARNING, "", "Example Warning"); 
} 

ConsoleUI::~ConsoleUI(){}

void ConsoleUI::render() {
    drawConsole(); 
}

void ConsoleUI::drawConsole() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    ImGui::Begin("Console");  
    ImGui::TextWrapped("Enter 'help' or '-h' for help");
    ImGui::Separator(); 
    ConsoleUI::readLogs(); 
    ImGui::Separator(); 
    drawTextInput(); 
    ImGui::End(); 
}

void ConsoleUI::readLogs() {
    const auto& logs = shbx::Logger::getLogs(); 

    for (const auto& log : logs) {
        int idx = std::min((int)log.level, 3); // clamp the index to avoid out-of-bounds

        ImGui::PushStyleColor(ImGuiCol_Text, LOG_COLORS[idx]); 
        ImGui::TextUnformatted(log.msg.c_str()); 
        ImGui::PopStyleColor(); 
    }
}

void ConsoleUI::drawTextInput() {
    static char str0[128] = "";
    ImGui::PushItemWidth(-FLT_MIN);       // expand input to size of window 
    ImGui::InputText("##", str0, IM_ARRAYSIZE(str0)); 
}