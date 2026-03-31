#include "SearchText.hpp"


bool SearchText::drawSearchUI(std::function<void(Match& match, char* replace)> onReplaceClick) {
    bool changed = false;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f); 
    
    ImVec2 currentPadding = ImGui::GetStyle().FramePadding;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(currentPadding.x, 4.0f)); 

    ImGui::BeginGroup(); 

    if (flags & SearchUIFlags::REPLACE) {
        if (ImGui::ArrowButton("##opts", showReplace ? ImGuiDir_Down : ImGuiDir_Right)) {
            showReplace = !showReplace;
        }
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(250); 
    if (ImGui::InputTextWithHint("##search", "Find...", inputBuffer, IM_ARRAYSIZE(inputBuffer))) {
        changed = true;
        isDirty = true;
    }
    
    if (flags & SearchUIFlags::CASE_SENSITIVE) {
        ImGui::SameLine();
        if (drawToggleButton("Aa", &caseSensitive, "Case Sensitive")) { changed = true; isDirty = true; }
    }
    if (flags & SearchUIFlags::MATCH_WHOLE_WORD) {
        ImGui::SameLine();
        if (drawToggleButton("Mw", &matchWholeWord, "Match Whole Word")) { changed = true; isDirty = true; }
    }
    if (flags & SearchUIFlags::REGEX) {
        ImGui::SameLine();
        if (drawToggleButton(".*", &useRegex, "Regular Expression")) { changed = true; isDirty = true; }
    }

    ImGui::SameLine(0, 35.0f); 
    ImGui::BeginDisabled(matches.empty());
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0)); 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.0f, 0.0f)); 
    
    if (ImGui::ArrowButton("##prev", ImGuiDir_Up)) {
        currentMatchIdx--;
        if (currentMatchIdx < 0) currentMatchIdx = (int)matches.size() - 1;
        requestScroll = true;
    }
    
    ImGui::SameLine(); 
    
    if (ImGui::ArrowButton("##next", ImGuiDir_Down)) {
        currentMatchIdx++;
        if (currentMatchIdx >= (int)matches.size()) currentMatchIdx = 0;
        requestScroll = true;
    }
    
    ImGui::PopStyleVar(); 
    ImGui::PopStyleColor(); 
    ImGui::EndDisabled();

    ImGui::SameLine(0, 10.0f); 
    if (!statusMessage.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", statusMessage.c_str());
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        if (!matches.empty()) {
            ImGui::Text("%d of %d", currentMatchIdx + 1, (int)matches.size());
        } else {
            ImGui::Text("No results");
        }
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();
    if ((flags & SearchUIFlags::REPLACE) && showReplace) {
        ImGui::BeginGroup();
        
        float offset = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.x;
        ImGui::Dummy(ImVec2(offset - ImGui::GetStyle().ItemSpacing.x, 0)); 
        ImGui::SameLine();
        ImGui::SetNextItemWidth(250); 
        ImGui::InputTextWithHint("##replace", "Replace with...", replaceBuffer, IM_ARRAYSIZE(replaceBuffer));
        ImGui::SameLine();
        
        ImGui::BeginDisabled(matches.empty() || currentMatchIdx == -1);
        if (ImGui::Button("Replace")) {
            if (onReplaceClick) {
                onReplaceClick(matches[currentMatchIdx], replaceBuffer);
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Replace All")) {
            if (onReplaceClick) {
                for (int i = static_cast<int>(matches.size()) - 1; i >= 0; i--) {
                    onReplaceClick(matches[i], replaceBuffer);
                }
            }
        }
        ImGui::EndDisabled();
        ImGui::EndGroup();
    }

    ImGui::PopStyleVar(3); 
    return changed;
}

bool SearchText::drawToggleButton(const char* label, bool* state, const char* tooltip) {
    bool changed = false;
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec4 activeColor = colors[ImGuiCol_ButtonActive];
    ImVec4 inactiveColor = ImVec4(0, 0, 0, 0);
    
    ImGui::PushStyleColor(ImGuiCol_Button, *state ? activeColor : inactiveColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[*state ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered]);
    
    if (ImGui::Button(label, ImVec2(32, 0))) { 
        *state = !*state;
        changed = true;
    }
    
    ImGui::PopStyleColor(2);
    
    if (ImGui::IsItemHovered() && tooltip) {
        ImGui::SetTooltip("%s", tooltip);
    }
    
    return changed;
}

void SearchText::setSearchFlag(SearchUIFlags newFlags) {
    flags = newFlags; 
}

