#pragma once 
#include "imgui.h"
#include <algorithm>
#include <cmath> 
#include <string>

// This only works if the text is monospaced however, the font that we use should already be monospaced.

// stores information about dragging the cursor with the text boxes
struct TextSelectionCtx {
    bool active = false; 
    int startRow= -1; 
    int endRow = -1; 
    int startCol = 0; 
    int endCol = 0; 

    // helper to handle dragging the cursor upwards 
    void getNormalizedRows(int& outMin, int& outMax) const {
        outMin = std::min(startRow, endRow); 
        outMax = std::max(startRow, endRow);
    }
}; 

struct TextSelectionMetrics {
    float lineHeight = 0.0f; 
    float charWidth = 0.0f; 
    float maxWidth = 0.0f;                  
    ImVec2 origin = ImVec2(0,0); 
    ImU32 Color = IM_COL32(0, 120, 215, 100);   // default blue like in vscode 
};

class TextSelector {
    public:
    static void drawRowSelection(ImDrawList* drawList, const TextSelectionCtx& ctx, const TextSelectionMetrics& metrics, int currRow); 
    static void calcTextRowAndCol(const ImVec2& mousePos, const TextSelectionMetrics& metrics, int& outRow, int& outCol);
    static void handleMouseClicks(TextSelectionCtx& ctx, int row, int col, const std::string& lineText); 
    static void getWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd); 
    private:
    static bool isWhiteSpace(char c); 
    static bool isDelimeter(char c); 
    static bool isToken(char c); 
};