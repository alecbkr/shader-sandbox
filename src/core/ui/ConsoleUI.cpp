#include "ConsoleUI.hpp"

ConsoleUI::ConsoleUI(){
    engine = std::make_shared<ConsoleEngine>(); 
} 

ConsoleUI::~ConsoleUI(){}

void ConsoleUI::render() {
    drawConsole(); 
}

void ConsoleUI::drawConsole() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_Once);
    
    ImGui::Begin("Console");  
    ImGui::TextWrapped("Enter 'help' for help");
    ImGui::Separator(); 
    {
        ImGui::BeginChild("ShowLogs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.95f), true);
        ConsoleUI::readLogs(); 
        ImGui::EndChild(); 
    }
    ImGui::Separator(); 
    drawTextInput(); 
    ImGui::End(); 
}

void ConsoleUI::readLogs() {
    if (!logSrc) return; 

    const auto& logs = logSrc->getLogs(); 

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

        ImGui::PushStyleColor(ImGuiCol_Text, LOG_COLORS[idx]); 
        ImGui::TextUnformatted(alert.c_str()); 
        ImGui::SameLine(0.0f, 0.0f); 
        ImGui::PopStyleColor(); 
        ImGui::TextUnformatted(log.msg.c_str()); 
        
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

void ConsoleUI::drawTextInput() {

    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue; 
    if (isFocused) {
        ImGui::SetKeyboardFocusHere(-1);    // target the next widget (text input box)
        isFocused = false;
    }

    // used to render input to bottom of window 
    float windowHeight = ImGui::GetWindowHeight(); 
    float inputHeight = ImGui::GetFrameHeight(); 
    ImGui::SetCursorPosY(windowHeight - inputHeight); 

    static char str0[128] = "";
    ImGui::PushItemWidth(-FLT_MIN);       // expand input to width of window 
    bool pressedEnter = ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputFlags); 

    if(pressedEnter) {
        std::string cmd = std::string(inputBuf); 

        if(!cmd.empty()) {
            executeCommand(); 
        }

        isFocused = true; 
    }
}

void ConsoleUI::executeCommand() {
    if(!engine) {
        return; 
    }

    std::string command(inputBuf); 
    Logger::addLog(LogLevel::INFO, ">", command, -1); 
    engine->processInput(command); 

    // reset the input buffer 
    std::fill(std::begin(inputBuf), std::end(inputBuf), '\0'); 
}