#include "ConsoleUI.hpp"

ConsoleUI::ConsoleUI(){
    logItems.push_back("Welcome to Shader Sandbox!"); 
} 

ConsoleUI::~ConsoleUI(){
    ConsoleUI::clearLogItems(); 
}

void ConsoleUI::render() {
    drawConsole(); 
}

void ConsoleUI::drawConsole() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    ImGui::Begin("Console");  
    ImGui::TextWrapped("Enter 'help' or '-h' for help");
    ImGui::Separator(); 
    drawConsoleLogs(); 
    ImGui::Separator(); 
    drawTextInput(); 
    ImGui::End(); 
}

void ConsoleUI::drawConsoleLogs() {
    for(const std::string item : logItems) {
         ImGui::TextUnformatted(item.c_str()); 
    }
}

void ConsoleUI::drawTextInput() {
    static char str0[128] = "Hello, world!";
    ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0)); 
}

void ConsoleUI::addLogItem(const char* item) {
    logItems.push_back(item); 
}

void ConsoleUI::clearLogItems() {
    logItems.clear(); 
}

