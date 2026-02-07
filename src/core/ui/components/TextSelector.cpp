#include "TextSelector.hpp"
#include <cctype>
#include <limits>

void TextSelector::drawRowSelection(ImDrawList* drawList, const TextSelectionCtx& ctx, const TextSelectionMetrics& metrics, int currRow) {
    if (!ctx.active) return; 

    int selMin, selMax; 
    ctx.getNormalizedRows(selMin, selMax); 
    if (currRow < selMin || currRow > selMax) return; 

    float hlStart = 0.0f; 
    float hlEnd = metrics.maxWidth; 

    // selecting a single line
    if (selMin == selMax) {
        int sCol = std::min(ctx.startCol, ctx.endCol); 
        int eCol = std::max(ctx.startCol, ctx.endCol); 
        hlStart = sCol * metrics.charWidth;
        hlEnd = (eCol + 1) * metrics.charWidth;    
    }

    // top line of multi-selection
    if (currRow == selMin) {
        int sCol = (ctx.startRow < ctx.endRow) ? ctx.startCol : ctx.endCol; 
        hlStart = sCol * metrics.charWidth; 
        hlEnd = metrics.maxWidth; 
    } 

    // bottom line of multi-selection
    else if (currRow == selMax) {
        int eCol = (ctx.startRow < ctx.endRow) ? ctx.endCol : ctx.startCol; 
        hlStart = 0.0f; 
        hlEnd = (eCol + 1) * metrics.charWidth; 
    }

    if (hlEnd > metrics.maxWidth) hlEnd = metrics.maxWidth; 

    // draw highlight box around the text 
    if (hlEnd > hlStart) {
        float lineY = metrics.origin.y + (currRow * metrics.lineHeight);

        drawList->AddRectFilled (
            ImVec2(metrics.origin.x + hlStart, lineY), 
            ImVec2(metrics.origin.x + hlEnd, lineY + metrics.lineHeight), 
            metrics.Color
        ); 
    }
}

void TextSelector::calcTextRowAndCol(const ImVec2& mousePos, const TextSelectionMetrics& metrics, int& outRow, int& outCol) {
    float relX = mousePos.x - metrics.origin.x; 
    float relY = mousePos.y - metrics.origin.y; 

    outRow = (int)std::floor(relY / metrics.lineHeight); 
    outCol = (int)std::floor(relX / metrics.charWidth); 

    if (outRow < 0) outRow = 0; 
    if (outCol < 0) outCol = 0; 
}

void TextSelector::handleMouseClicks(TextSelectionCtx& ctx, int row, int col, const std::string& lineText) {
    int clickCount = ImGui::GetMouseClickedCount(0); 

    ctx.active = true; 
    ctx.startRow = row; 
    ctx.endRow = row; 

    // select the entire line if 3x click 
    if (clickCount == 3) {
        ctx.startCol = 0; 
        ctx.endCol = std::numeric_limits<int>::max(); 
    }

    // select a string if 2x click 
    else if (clickCount == 2) {
        int wStart, wEnd; 
        getWordUnderCursor(lineText, col, wStart, wEnd); 
        ctx.startCol = wStart; 
        ctx.endCol = wEnd; 
    }

    // single click 
    else {
        ctx.startCol = col; 
        ctx.endCol = col; 
    }
}

void TextSelector::getWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd) {
    if (text.empty()) {
        outStart = 0; outEnd = 0; 
        return; 
    }

    col = std::clamp(col, 0, (int)text.length() - 1); 
    char clickedChar = text[col]; 
     
    if (isWhiteSpace(clickedChar)) {
        outStart = col; 
        while(outStart > 0 && isWhiteSpace(text[outStart - 1])) {
            outStart --; 
        }
        outEnd = col; 
        while(outEnd < text.length() - 1 && isWhiteSpace(text[outEnd + 1])) {
            outEnd ++; 
        }
    }

    else if (isDelimeter(clickedChar)) {
        outStart = col; 
        outEnd = col; 
    }

    else {
        outStart = col; 
        while(outStart > 0 && isToken(text[outStart -1])) {
            outStart --;
        }

        outEnd = col; 
        while(outEnd < text.length() -1 && isToken(text[outEnd + 1])) {
            outEnd ++; 
        }
    }

}

// helpers for mouse 2x clicking selection to decide how much of the substr to select 
bool isWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n'; 
}

bool isDelimeter(char c) {
    return c == '"' || c == '\'' || c == '(' || c == ')' ||
            c == '[' || c == ']'  || c == '{' || c == '}'; 
}

bool TextSelector::isToken(char c) {
    return !isWhiteSpace(c) && !isDelimeter(c); 
} 