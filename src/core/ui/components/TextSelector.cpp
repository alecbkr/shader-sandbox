#include "TextSelector.hpp"
#include <cctype>
#include <limits>
#include <iostream>

static struct TextSelectorState {
    TextSelectionCtx* userCtx = nullptr; 

    ImVec2 origin; 
    float lineHeight; 
    float charWidth; 
    float maxWidth; 
    ImU32 highlightColor; 

    bool isPendingWordSelect = false; 
    int hoveredRow = -1; 
    int hoveredCol = -1; 
} txtState;

bool TextSelector::Begin(const char* id, int totalRows, TextSelectionCtx& ctx, ImU32 highlightCol) {
    if (!isFontMonospace()) return false; 

    txtState.userCtx = &ctx; 
    txtState.lineHeight = std::max(1.0f, ImGui::GetTextLineHeight()); 
    txtState.charWidth = ImGui::CalcTextSize("A").x; 
    txtState.maxWidth = ImGui::GetContentRegionAvail().x; 
    txtState.isPendingWordSelect = false; 

    // draw the invisible "hitbox" that we use to select from 
    float totalHeight = (float)totalRows *txtState.lineHeight; 
    ImGui::InvisibleButton(id, ImVec2(txtState.maxWidth, totalHeight)); 

    // capture the screen pos of the content area 
    txtState.origin = ImGui::GetItemRectMin(); 

    // handle mouse clicks 
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImVec2 mouse = ImGui::GetMousePos(); 
        float relY = mouse.y - txtState.origin.y;
        float relX = mouse.x - txtState.origin.x; 

        txtState.hoveredRow = (int)std::floor(relY / txtState.lineHeight); 
        txtState.hoveredCol = (int)std::floor(relX / txtState.charWidth); 

        if (txtState.hoveredRow < 0) txtState.hoveredRow = 0; 
        if (txtState.hoveredRow >= totalRows) txtState.hoveredRow = totalRows - 1; 
        if (txtState.hoveredCol < 0) txtState.hoveredCol = 0; 

        if (ImGui::IsMouseClicked(0)) {
            int clicks = ImGui::GetMouseClickedCount(0); 

            ctx.isActive = true; 
            ctx.isCharMode = true; 
            ctx.startRow = txtState.hoveredRow; 
            ctx.endRow = txtState.hoveredRow; 

            // select the entire line
            if (clicks == 3) {
                ctx.startCol = 0;
                ctx.endCol = std::numeric_limits<int>::max(); 
            }

            // select a whole string 
            else if (clicks == 2) {
                txtState.isPendingWordSelect = true; 
            }

            else {
                ctx.startCol = txtState.hoveredCol; 
                ctx.endCol = txtState.hoveredCol; 
            }
        }

        // handle the dragging of the mouse to select text 
        else if (ImGui::IsMouseDown(0) && ctx.isActive && !ImGui::IsMouseClicked(0)) {
            ctx.endCol = txtState.hoveredCol; 
            ctx.endRow = txtState.hoveredRow; 
        }

    } 
    return true; 
}

void TextSelector::Text(const std::string& text, int rowIndex, std::function<void()> customDraw) {
    if (!txtState.userCtx) {
        std::cerr << "Must Call 'TextSelector::Begin()' to draw the text box" << std::endl;  
        return; 
    } 

    // handle the double click logic and calculate the string that we need to select 
    if (txtState.isPendingWordSelect && rowIndex == txtState.hoveredRow) {
        int start, end; 
        getWordUnderCursor(text, txtState.hoveredCol, start, end); 
        txtState.userCtx->startCol = start; 
        txtState.userCtx->endCol = end; 
        txtState.isPendingWordSelect = false; 
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList(); 
    float lineY = txtState.origin.y + (rowIndex * txtState.lineHeight); 

    // draw the selection highlight around the text 
    if (txtState.userCtx->isActive) {
        int relMin = std::min(txtState.userCtx->startRow, txtState.userCtx->endRow); 
        int relMax = std::max(txtState.userCtx->startRow, txtState.userCtx->endRow); 

        if (rowIndex >= relMin && rowIndex <= relMax) {
            float hlStart, hlEnd = 0.0f; 

            // normalization of columns based on the direction of the rows 
            int sCol = txtState.userCtx->startCol; 
            int eCol = txtState.userCtx->endCol; 
            if (txtState.userCtx->startRow > txtState.userCtx->endRow) std::swap(sCol, eCol); {
                
                // single line selection
                if (relMin == relMax) {
                    hlStart = std::min(sCol, eCol) * txtState.charWidth;
                    hlEnd = (std::max(sCol, eCol) + 1) * txtState.charWidth;
                }

                // top row of multi-line selection
                else if (rowIndex == relMin) {
                    hlStart = sCol * txtState.charWidth; 
                    hlEnd =txtState.maxWidth; 
                }

                // bottom row of mult-line selection
                else if (rowIndex == relMax) {
                    hlStart = 0.0f; 
                    hlEnd = (eCol + 1) * txtState.charWidth; 
                }

                // middle row of multi-line 
                else {
                    hlStart = 0.0f; 
                    hlEnd = txtState.maxWidth; 
                }

                float textW = (text.length() + 1) * txtState.charWidth; 
                if (hlEnd > textW) hlEnd = textW; 

                if (hlEnd > hlStart) {
                    drawList->AddRectFilled(
                        ImVec2(txtState.origin.x + hlStart, lineY), 
                        ImVec2(txtState.origin.x + hlEnd, lineY + txtState.lineHeight), 
                        txtState.highlightColor
                    ); 
                }
            }
        }
    }

    ImGui::SetCursorScreenPos(ImVec2(txtState.origin.x, lineY)); 

    // here we execute the callback fn that allows use custom commands (e.g. have colored text segment)
    if (customDraw) {
        customDraw();
    } else {
        ImGui::TextUnformatted(text.c_str()); 
    }
}

void TextSelector::End() {
    txtState.userCtx = nullptr; 
}

float TextSelector::GetLineHeight() {
    return std::max(1.0f, ImGui::GetTextLineHeight());
}

// selection based on mouse clicks (e.g. two to select a string and three to select the entire line)
void TextSelector::handleMouseClicks(TextSelectionCtx& ctx, int row, int col, const std::string& lineText) {
    int clickCount = ImGui::GetMouseClickedCount(0); 

    ctx.isActive = true; 
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
bool TextSelector::isWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n'; 
}
bool TextSelector::isDelimeter(char c) {
    return c == '"' || c == '\'' || c == '(' || c == ')' ||
            c == '[' || c == ']'  || c == '{' || c == '}'; 
}
bool TextSelector::isToken(char c) {
    return !isWhiteSpace(c) && !isDelimeter(c); 
} 

// get the narrowest and widest characters and check the spacing between the two to see if the current font is monospace
bool TextSelector::isFontMonospace() {
    float w = ImGui::CalcTextSize("W").x;
    float i = ImGui::CalcTextSize("i").x;
    return std::abs(w - i) < 0.001f;  
}