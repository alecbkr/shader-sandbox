#include "TextSelector.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

TextSelector::CurrentState TextSelector::state; 

// setups the draw state to begin drawing the selection boxes 
bool TextSelector::Begin(const char* id, int totalRows, TextSelectionCtx& ctx, TextSelectorLayout& layout) {
    ImGui::PushID(id); 

    layout.lineHeight = std::max(1.0f, ImGui::GetTextLineHeight()); 
    layout.charWidth = ImGui::CalcTextSize("A").x; 
    layout.maxWidth = ImGui::GetContentRegionAvail().x; 
    layout.highlightColor = IM_COL32(0, 120, 215, 100); 
    layout.origin = ImGui::GetCursorScreenPos(); 

    float totalHeight = (float)totalRows * layout.lineHeight; 
    ImGui::SetNextItemAllowOverlap();
    ImGui::InvisibleButton("##InputLayer", ImVec2(layout.maxWidth, totalHeight));

    if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput); 
        handleInput(totalRows, ctx, layout); 
    }

    state.ctx = &ctx; 
    state.layout = layout; 
    state.totalRows = totalRows; 
    state.currRow = 0; 
    state.isActive = true; 

    return true; 
}

// renders a normal ImGui::TextUnformatted
void TextSelector::Text(const std::string& rawText) {
    Text(rawText, [&rawText](){
        ImGui::TextUnformatted(rawText.c_str()); 
    });
}

// renders the callback function where you can set different types of ImGui text widgets (e.g. having multi-colored text)
void TextSelector::Text(const std::string& rawText, std::function<void()> drawCallback) {
    if (!state.isActive) {
        if (drawCallback) drawCallback(); 
        return; 
    }

    TextSelectionCtx* ctx = state.ctx; 
    const TextSelectorLayout& layout = state.layout; 
    int rowIdx = state.currRow; 
    
    if (ctx->isActive && ctx->wordRecalc && rowIdx == ctx->startRow) {
        int start, end; 
        getWordUnderCursor(rawText, ctx->startCol, start, end); 
        ctx->startCol = start; 
        ctx->endCol = end; 
        ctx->wordRecalc = false; 
    }

    float lineY = layout.origin.y + (rowIdx * layout.lineHeight);           // setup drawing the highlight
    // handles drawing the actual highlight box around the text 
    if (ctx->isActive) {
        int rMin = std::min(ctx->startRow, ctx->endRow);
        int rMax = std::max(ctx->startRow, ctx->endRow);

        if (rowIdx >= rMin && rowIdx <= rMax) {
            float hlStart = 0.0f;
            float hlEnd = 0.0f;
            
            // Triple clicking to select the entire line 
            if (ctx->mode == SelectionMode::Line) {
                hlStart = 0.0f; 
                hlEnd = layout.maxWidth; 
            } else {
                int cStart = ctx->startCol; 
                int cEnd = ctx->endCol; 

                // normalize the columns for highlighting backwards 
                if (ctx->startRow > ctx->endRow) std::swap(cStart, cEnd); 
                if (ctx->startRow == ctx->endRow && cStart > cEnd) std::swap(cStart, cEnd); 

                // single line selection
                if (rMin == rMax) {
                    hlStart = cStart * layout.charWidth; 
                    hlEnd = cEnd * layout.charWidth; 
                } 
                
                // start of the multiline 
                else if (rowIdx == rMin) {
                    int col = (ctx->startRow < ctx->endRow) ? ctx->startCol : ctx->endCol; 
                    hlStart = col * layout.charWidth; 
                    hlEnd = layout.maxWidth; 
                } 
                
                // end of the multiline
                else if (rowIdx == rMax) {
                    hlStart = 0.0f; 
                    int col = (ctx->startRow < ctx->endRow) ? ctx->endCol : ctx->startCol; 
                    hlEnd = col * layout.charWidth; 
                }
                
                // middle lines 
                else {
                    hlStart = 0.0f; 
                    hlEnd = layout.maxWidth; 
                }
            }

            // draws the actual rectangle text highlighting 
            if (hlEnd > hlStart) {
                ImDrawList* drawList = ImGui::GetWindowDrawList(); 
                drawList->AddRectFilled(
                    ImVec2(layout.origin.x + hlStart, lineY), 
                    ImVec2(layout.origin.x + hlEnd, lineY + layout.lineHeight), 
                    layout.highlightColor
                ); 
            }
            
        }
    }

    ImGui::SetCursorScreenPos(ImVec2(layout.origin.x, lineY)); 

    if (drawCallback) {
        drawCallback(); 
    }

    state.currRow ++; 

}

// resets the current active state
void TextSelector::End() {
    if (state.isActive) {
        state.isActive = false; 
        state.ctx = nullptr; 
        ImGui::PopID(); 
    }
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
            ctx.mode = SelectionMode::Line; 
            ctx.startCol = 0; 
            ctx.endCol = std::numeric_limits<int>::max(); 
        } else if (clicks == 2) { 
            ctx.mode = SelectionMode::Word; 
            ctx.startCol = col; 
            ctx.endCol = col; 
            ctx.wordRecalc = false; 
        } else { 
            ctx.mode = SelectionMode::Normal; 
            ctx.startCol = col; 
            ctx.endCol = col;
            ctx.wordRecalc = false; 
        }
    } 

    // handle drag
    else if (ImGui::IsMouseDown(0) && ctx.isActive) {
        ctx.endRow = row;
        ctx.endCol = col;

        if (ctx.mode == SelectionMode::Normal) {
            ctx.endCol = col; 
        }
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