#pragma once 
#include "imgui.h"
#include <algorithm>
#include <cmath> 
#include <string>
#include <functional>

// This only works if the text is monospaced however, the font that we use should already be monospaced.

// stores information about dragging the cursor with the text boxes
struct TextSelectionCtx {
    bool isActive = false; 
    bool isCharMode = false; 

    int startRow= -1; 
    int endRow = -1; 

    int startCol = 0; 
    int endCol = 0; 

    void clear() {
        isActive = false; 
        isCharMode = false; 
        startRow = -1; 
        endRow = -1; 
        startCol = 0; 
        endCol = 0; 
    }

    // helper to handle dragging the cursor upwards 
    std::pair<int, int> getRange() const {
        if (startRow <= endRow) 
            return {startRow, endRow};
    }
}; 

// ImGui extension that I made to spawn ImGui::Text that we can select and copy the contents of 
class TextSelector {
    public:
    static bool Begin(const char* id, int totalRows, TextSelectionCtx& ctx, ImU32 highlightCol = IM_COL32(0, 120, 215, 100));
    static void Text(const std::string& text, int rowIndex, std::function<void()> customDraw); 
    static void End(); 
    static float GetLineHeight(); 
    static void copyText(); 
    static std::string getSelectedText(); 
    private:
    static bool isWhiteSpace(char c); 
    static bool isDelimeter(char c); 
    static bool isToken(char c); 
    static void handleMouseClicks(TextSelectionCtx& ctx, int row, int col, const std::string& lineText); 
    static void getWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd); 
    static bool isFontMonospace(); 
};