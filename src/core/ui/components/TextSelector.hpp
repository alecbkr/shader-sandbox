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
    bool isCharMode = false; 
    int startRow = -1; 
    int endRow = -1; 
    int startCol = 0; 
    int endCol = 0; 
    
    // NEW: Store exact pixel coordinates for proportional font math
    float startMouseX = 0.0f; 
    float endMouseX = 0.0f; 

    void clear() {
        isActive = false; 
        mode = SelectionMode::Normal;
        wordRecalc = false; 
        startRow = -1; endRow = -1; 
        startCol = 0; endCol = 0; 
        startMouseX = 0.0f; 
        endMouseX = 0.0f; 
    }
}; 

struct TextSelectorLayout {
    ImVec2 origin; 
    float lineHeight; 
    float charWidth; 
    float maxWidth; 
    ImU32 highlightColor; 
};

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
        int currRow = 0; 
        int totalRows = 0; 
        bool isActive = false; 
    }; 

    static CurrentState state; 

    static void handleInput(int totalRows, TextSelectionCtx& ctx, const TextSelectorLayout& layout);
    static void getWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd);
    
    // NEW: Helper to find exact character index from pixel offset
    static int getExactColumn(const std::string& text, float targetX);

    static bool isWhiteSpace(char c); 
    static bool isDelimeter(char c); 
    static bool isToken(char c); 
};