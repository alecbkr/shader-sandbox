#pragma once 
#include "imgui.h"
#include <string>
#include <functional>
#include <vector>

enum class SelectionMode {
    Normal,
    Word,
    Line
};

struct TextSelectionCtx {
    SelectionMode mode = SelectionMode::Normal;
    bool wordRecalc; 
    bool isActive = false; 
    bool isCharMode = false; // true = select chars, false = select full line
    int startRow = -1; 
    int endRow = -1; 
    int startCol = 0; 
    int endCol = 0; 

    void clear() {
        isActive = false; 
        mode = SelectionMode::Normal;
        wordRecalc = false; 
        startRow = -1; endRow = -1; 
        startCol = 0; endCol = 0; 
    }
}; 

struct TextSelectorLayout {
    ImVec2 origin; 
    float lineHeight; 
    float charWidth; 
    float maxWidth; 
    ImU32 highlightColor; 
};

// ImGui extension that I made to spawn ImGui::Text that we can select and copy the contents of 

class TextSelector {
public:
    static bool Begin(const char* id, int totalRows, TextSelectionCtx& ctx, TextSelectorLayout& layout);
    static void Text(const std::string& rawText); 
    static void Text(const std::string& rawText, std::function<void()> drawCallback);
    static void End(); 
    static void copyText(const TextSelectionCtx& ctx, int totalRows, std::function<std::string(int)> fetchLine);
    
private:
    struct CurrentState {
        TextSelectionCtx* ctx = nullptr; 
        TextSelectorLayout layout; 
        // int currRow = 0; 
        int totalRows = 0; 
        bool isActive = false; 
    }; 

    static CurrentState state; 

    static void handleInput(int totalRows, TextSelectionCtx& ctx, const TextSelectorLayout& layout);
    static void getWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd);
    static bool isWhiteSpace(char c); 
    static bool isDelimeter(char c); 
    static bool isToken(char c); 
};