#pragma once 
#include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>
#include <string>



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

    void drawConsole(); 
    void drawTextInput(); 
}; 