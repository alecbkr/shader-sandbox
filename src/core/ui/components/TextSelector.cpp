#include "TextSelector.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

TextSelector::CurrentState TextSelector::state; 

// setups the draw state to begin drawing the selection boxes 
bool TextSelector::Begin(const char* id, int totalRows, TextSelectionCtx& ctx, TextSelectorLayout& layout) {
    ImGui::PushID(id); 

    layout.lineHeight = std::max(1.0f, ImGui::GetTextLineHeightWithSpacing()); 
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

    ImGui::SetCursorScreenPos(layout.origin); 

    state.ctx = &ctx; 
    state.layout = layout; 
    state.totalRows = totalRows; 
    state.currRow = 0; 
    state.isActive = true; 

    return true; 
}

void TextSelector::Text(const std::string& rawText) {
    Text(rawText, [&rawText](){
        ImGui::TextUnformatted(rawText.c_str()); 
    });
}

void TextSelector::Text(const std::string& rawText, std::function<void()> drawCallback) {
    if (!state.isActive) {
        if (drawCallback) drawCallback(); 
        return; 
    }

    TextSelectionCtx* ctx = state.ctx; 
    const TextSelectorLayout& layout = state.layout; 
    int rowIdx = state.currRow; 

    if (ctx->isActive) {
        if (rowIdx == ctx->startRow && (ctx->mode == SelectionMode::Normal || ctx->wordRecalc)) {
            ctx->startCol = getExactColumn(rawText, ctx->startMouseX);
        }
        if (rowIdx == ctx->endRow && (ctx->mode == SelectionMode::Normal || ctx->wordRecalc)) {
            ctx->endCol = getExactColumn(rawText, ctx->endMouseX);
        }
    }


    if (ctx->isActive && ctx->wordRecalc && rowIdx == ctx->startRow) {
        int start, end; 
        getWordUnderCursor(rawText, ctx->startCol, start, end); 
        ctx->startCol = start; 
        ctx->endCol = end; 
        ctx->wordRecalc = false; 
    }

    // handles drawing the actual highlight box around the text 
    if (ctx->isActive) {
        int rMin = std::min(ctx->startRow, ctx->endRow);
        int rMax = std::max(ctx->startRow, ctx->endRow);

        if (rowIdx >= rMin && rowIdx <= rMax) {
            float hlStart = 0.0f;
            float hlEnd = 0.0f;
            int txtLength = (int)rawText.length();
            
            // Helper to get actual pixel bounds
            auto getPixelWidth = [&](int charIndex) -> float {
                int len = std::clamp(charIndex, 0, txtLength);
                return ImGui::CalcTextSize(rawText.c_str(), rawText.c_str() + len).x;
            };

            float fullTextWidth = getPixelWidth(txtLength);
            
            // Triple clicking to select the entire line 
            if (ctx->mode == SelectionMode::Line) {
                hlStart = 0.0f; 
                hlEnd = fullTextWidth; 
            } 
            else {
                int cStart = ctx->startCol; 
                int cEnd = ctx->endCol; 

                // normalize the columns for highlighting backwards
                if (ctx->startRow > ctx->endRow) std::swap(cStart, cEnd); 
                if (ctx->startRow == ctx->endRow && cStart > cEnd) std::swap(cStart, cEnd); 

                // single line selection
                if (rMin == rMax) {
                    hlStart = getPixelWidth(cStart); 
                    hlEnd = getPixelWidth(cEnd); 
                } 
                // start of the multiline 
                else if (rowIdx == rMin) {
                    int col = (ctx->startRow < ctx->endRow) ? ctx->startCol : ctx->endCol; 
                    hlStart = getPixelWidth(col); 
                    hlEnd = fullTextWidth; 
                } 
                // end of the multiline
                else if (rowIdx == rMax) {
                    hlStart = 0.0f; 
                    int col = (ctx->startRow < ctx->endRow) ? ctx->endCol : ctx->startCol; 
                    hlEnd = getPixelWidth(col); 
                }
                // middle lines 
                else {
                    hlStart = 0.0f; 
                    hlEnd = fullTextWidth; 
                }
            }
            // draws the actual rectangle text highlighting 
            if (hlEnd > hlStart) {
                ImVec2 cursorPos = ImGui::GetCursorScreenPos(); 
                ImDrawList* drawList = ImGui::GetWindowDrawList(); 
                drawList->AddRectFilled(
                    ImVec2(layout.origin.x + hlStart, cursorPos.y), 
                    ImVec2(layout.origin.x + hlEnd, cursorPos.y + layout.lineHeight), 
                    layout.highlightColor
                ); 
            }
        }
    }

    if (drawCallback) {
        drawCallback(); 
    }

    state.currRow++; 
}

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
    int colFallback = (int)std::floor(relX / layout.charWidth);

    if (row < 0) row = 0;
    if (row >= totalRows) row = totalRows - 1;
    if (colFallback < 0) colFallback = 0;

    // handle clicks
    if (ImGui::IsMouseClicked(0)) {
        int clicks = ImGui::GetMouseClickedCount(0);
        ctx.isActive = true;
        ctx.startRow = row; ctx.endRow = row;
        ctx.startMouseX = relX; ctx.endMouseX = relX; 

        if (clicks == 3) { 
            ctx.mode = SelectionMode::Line; 
            ctx.startCol = 0; 
            ctx.endCol = std::numeric_limits<int>::max(); 
        } else if (clicks == 2) { 
            ctx.mode = SelectionMode::Word; 
            ctx.startCol = colFallback; 
            ctx.endCol = colFallback; 
            ctx.wordRecalc = true; 
        } else { 
            ctx.mode = SelectionMode::Normal; 
            ctx.startCol = colFallback; 
            ctx.endCol = colFallback;
            ctx.wordRecalc = false; 
        }
    } 
    // handle drag
    else if (ImGui::IsMouseDown(0) && ctx.isActive) {
        ctx.endRow = row;
        ctx.endMouseX = relX;

        if (ctx.mode == SelectionMode::Normal) {
            ctx.endCol = colFallback; 
        }
    }
}

int TextSelector::getExactColumn(const std::string& text, float targetX) {
    if (targetX <= 0.0f) return 0;
    int len = (int)text.length();
    float lastW = 0.0f;
    
    for (int i = 1; i <= len; ++i) {
        float w = ImGui::CalcTextSize(text.c_str(), text.c_str() + i).x;
        if (w >= targetX) {
            // Pick whichever boundary is closest to the mouse pixel
            if ((targetX - lastW) < (w - targetX)) return i - 1;
            return i;
        }
        lastW = w;
    }
    return len;
}

void TextSelector::copyText(const TextSelectionCtx& ctx, int totalRows, std::function<std::string(int)> fetchLine) {
    if (!ctx.isActive) return;
    
    int topRow = ctx.startRow; 
    int topCol = ctx.startCol; 
    int botRow = ctx.endRow; 
    int botCol = ctx.endCol; 

    if (topRow > botRow || (topRow == botRow && topCol > botCol)) {
        std::swap(topRow, botRow); 
        std::swap(topCol, botCol); 
    }

    topRow = std::max(0, topRow); 
    botRow = std::min(totalRows - 1, botRow); 

    std::stringstream ss; 

    for (int i = topRow; i <= botRow; ++i) {
        std::string line = fetchLine(i); 
        int len = (int)line.length(); 

        if (ctx.mode == SelectionMode::Line) {
            ss << line; 
        } else {
            if (topRow == botRow) {
                int start = std::clamp(topCol, 0, len); 
                int end = std::clamp(botCol, 0, len); 
                ss << line.substr(start, end - start); 
            }
            else if (i == topRow) {
                int start = std::clamp(topCol, 0, len); 
                ss << line.substr(start); 
            }
            else if (i == botRow) {
                int end = std::clamp(botCol, 0, len); 
                ss << line.substr(0, end); 
            }
            else {
                ss << line; 
            }
        }

        if (i != botRow) {
            ss << "\n";
        }
    }

    if (ss.rdbuf()->in_avail() > 0) {
        ImGui::SetClipboardText(ss.str().c_str()); 
    }
}

// helpers for mouse 2x clicking selection to decide how much of the substr to select 
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
        while(outEnd < text.length() && isWhiteSpace(text[outEnd])) outEnd++; 
    }
    else if (isDelimeter(clickedChar)) {
        outStart = col; 
        outEnd = col + 1; 
    }
    else {
        outStart = col; 
        while(outStart > 0 && isToken(text[outStart -1])) outStart--;
        outEnd = col; 
        while(outEnd < text.length() && isToken(text[outEnd])) outEnd++; 
    }
}
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