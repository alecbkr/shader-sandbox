#include "TextSelector.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

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

bool TextSelector::Begin(const char* id, int totalRows, TextSelectionCtx& ctx, TextSelectorLayout& layout) {
    layout.lineHeight = std::max(1.0f, ImGui::GetTextLineHeight());
    layout.charWidth = ImGui::CalcTextSize("A").x; 
    layout.maxWidth = ImGui::GetContentRegionAvail().x;
    layout.highlightColor = IM_COL32(0, 120, 215, 100); // Default VScode Blue

    float totalHeight = (float)totalRows * layout.lineHeight;
    
    ImGui::SetNextItemAllowOverlap(); 
    ImGui::InvisibleButton(id, ImVec2(layout.maxWidth, totalHeight));

    layout.origin = ImGui::GetItemRectMin(); 
    
    if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        handleInput(totalRows, ctx, layout);
    }

    // handle mouse clicks 
    if (ImGui::IsMouseClicked(0)) {
        int clicks = ImGui::GetMouseClickedCount(0); 
        ctx.isActive = true; 
        ctx.startRow = txtState.hoveredRow; 
        ctx.endRow = txtState.hoveredRow; 

      
            if (clicks == 3) {
                ctx.mode = SelectionMode::Line;
                ctx.startCol = 0;
                ctx.endCol = std::numeric_limits<int>::max(); 
            }

            else if (clicks == 2) {
                ctx.mode = SelectionMode::Word;
                txtState.isPendingWordSelect = true;
            }
            else {
                ctx.mode = SelectionMode::Normal;
                ctx.startCol = txtState.hoveredCol; 
                ctx.endCol = txtState.hoveredCol; 
            }
    }    
    
    else if (ImGui::IsMouseClicked(0) && ctx.isActive) {
        ctx.endRow = txtState.hoveredRow; 

        if (ctx.mode == SelectionMode::Line) {
            ctx.startCol = 0; 
            ctx.endCol = std::numeric_limits<int>::max(); 
        }

        else if (ctx.mode == SelectionMode::Word) {
            if (txtState.hoveredCol != ctx.startCol) {
                ctx.mode = SelectionMode::Normal; 
                ctx.endCol = txtState.hoveredCol; 
            }
        }

        else {
            ctx.endCol = txtState.hoveredCol; 
        }
    }

    return true;
}

void TextSelector::Text(int rowIndex, const std::string& lineContent, const TextSelectionCtx& ctx, const TextSelectorLayout& layout, std::function<void()> drawCallback) {
    
    float lineY = layout.origin.y + (rowIndex * layout.lineHeight);
    ImGui::SetCursorScreenPos(ImVec2(layout.origin.x, lineY));

    // handles drawing the highlight around the text 
    if (ctx.isActive) {
        int rMin = std::min(ctx.startRow, ctx.endRow);
        int rMax = std::max(ctx.startRow, ctx.endRow);

        if (rowIndex >= rMin && rowIndex <= rMax) {
            float hlStart = 0.0f;
            float hlEnd = layout.maxWidth;

            int cStart = ctx.startCol;
            int cEnd = ctx.endCol;
            
            if (ctx.startRow > ctx.endRow) std::swap(cStart, cEnd);
            if (ctx.startRow == ctx.endRow && cStart > cEnd) std::swap(cStart, cEnd);

            bool hasSelection = (rMin != rMax) || (cStart != cEnd);

            if (hasSelection) {
                if (rMin == rMax) { 
                    hlStart = cStart * layout.charWidth;
                    hlEnd = cEnd * layout.charWidth; 
                                       
                } else if (rowIndex == rMin) { 
                    int activeCol = (ctx.startRow < ctx.endRow) ? ctx.startCol : ctx.endCol;
                    hlStart = activeCol * layout.charWidth;
                } else if (rowIndex == rMax) { 
                    int activeCol = (ctx.startRow < ctx.endRow) ? ctx.endCol : ctx.startCol;
                    hlEnd = activeCol * layout.charWidth; 
                }

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(
                    ImVec2(layout.origin.x + hlStart, lineY),
                    ImVec2(layout.origin.x + hlEnd, lineY + layout.lineHeight),
                    layout.highlightColor
                );
            }
        }
    }

    if (drawCallback) {
        drawCallback();
    } else {
        ImGui::TextUnformatted(lineContent.c_str());
    }
}

void TextSelector::End() {
}

void TextSelector::handleInput(int totalRows, TextSelectionCtx& ctx, const TextSelectorLayout& layout) {
    ImVec2 mouse = ImGui::GetMousePos();
    float relY = mouse.y - layout.origin.y;
    float relX = mouse.x - layout.origin.x;

    int row = (int)std::floor(relY / layout.lineHeight);
    int col = (int)std::floor(relX / layout.charWidth);

    if (row < 0) row = 0;
    if (row >= totalRows) row = totalRows - 1;
    if (col < 0) col = 0;

    // handle clicks
    if (ImGui::IsMouseClicked(0)) {
        int clicks = ImGui::GetMouseClickedCount(0);
        ctx.isActive = true;
        ctx.startRow = row; ctx.endRow = row;

        if (clicks == 3) { 
            ctx.startCol = 0; 
            ctx.endCol = 10000; 
        } else if (clicks == 2) { 
            ctx.startCol = col; ctx.endCol = col; 
        } else { 
            ctx.startCol = col; ctx.endCol = col;
        }
    } 

    // handle drag
    else if (ImGui::IsMouseDown(0) && ctx.isActive) {
        ctx.endRow = row;
        ctx.endCol = col;
    }
}

void TextSelector::copyText(const TextSelectionCtx& ctx, int totalRows, std::function<std::string(int)> fetchLine) {
    if (!ctx.isActive) return;
    
    std::stringstream ss;
    int rStart = std::min(ctx.startRow, ctx.endRow);
    int rEnd = std::max(ctx.startRow, ctx.endRow);
    
    // clamp to valid range
    rStart = std::max(0, rStart);
    rEnd = std::min(totalRows - 1, rEnd);

    for (int i = rStart; i <= rEnd; ++i) {
        std::string line = fetchLine(i);
        
        ss << line;
        if (i != rEnd) ss << "\n";
    }
    
    if (ss.rdbuf()->in_avail() > 0)
        ImGui::SetClipboardText(ss.str().c_str());
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
        while(outStart > 0 && isWhiteSpace(text[outStart - 1])) outStart--; 
        outEnd = col; 
        while(outEnd < text.length() - 1 && isWhiteSpace(text[outEnd + 1])) outEnd++; 
    }
    else if (isDelimeter(clickedChar)) {
        outStart = col; 
        outEnd = col; 
    }
    else {
        outStart = col; 
        while(outStart > 0 && isToken(text[outStart -1])) outStart--;
        outEnd = col; 
        while(outEnd < text.length() -1 && isToken(text[outEnd + 1])) outEnd++; 
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