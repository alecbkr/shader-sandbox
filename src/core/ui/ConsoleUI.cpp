#include "ConsoleUI.hpp"

ConsoleUI::ConsoleUI(){}; 

ConsoleUI::~ConsoleUI(){};

void ConsoleUI::render() {
    ImGui::Text("Debug Console");

    drawConsole(); 
}

void ConsoleUI::drawConsole() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    ImGui::Begin("Console");  
    drawTextInput(); 

    ImGui::End(); 
}
void ConsoleUI::drawTextInput() {
    static char str0[128] = "Hello, world!";
    ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0)); 
}
