#pragma once 
#include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <vector>



// Adapted from imgui_demo.cpp
class ConsoleUI {
    public: 
    ConsoleUI();    
    ~ConsoleUI();  

    ConsoleUI(const ConsoleUI&)  = delete; 
    ConsoleUI& operator = (const ConsoleUI&) = delete; 

    void render();
     
    private: 
    char InputBuf[256]; 
    // ImVector<char *> items; 
    // ImVector<char *> history; 
    // ImVector<const char*> Commands; 
    std::vector<std::string> logItems; 
    std::vector<std::string> history; 
    std::vector<const std::string> commands; 

    int historyPos; 

    void drawConsole(); 
    void drawConsoleLogs(); 
    void drawTextInput(); 
    void clearLogItems(); 
    void addLogItem(const char*); 
}; 