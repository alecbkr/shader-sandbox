#include "SearchText.hpp"

bool SearchText::drawSearchUI(std::function<void()> onReplaceClick) {
    bool changed = false;

    if (flags & SearchUIFlags::REPLACE) {
        if (ImGui::ArrowButton("##opts", showReplace ? ImGuiDir_Down : ImGuiDir_Right)) {
            showReplace = !showReplace;
        }
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(150);
    if (ImGui::InputTextWithHint("##search", "Find...", inputBuffer, IM_ARRAYSIZE(inputBuffer))) {
        changed = true;
        isDirty = true;
    }
    ImGui::SameLine();

    ImGui::BeginDisabled(matches.empty());
    if (ImGui::Button("<")) {
        currentMatchIdx--;
        if (currentMatchIdx < 0) currentMatchIdx = (int)matches.size() - 1;
        requestScroll = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(">")) {
        currentMatchIdx++;
        if (currentMatchIdx >= matches.size()) currentMatchIdx = 0;
        requestScroll = true;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (!statusMessage.empty()) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", statusMessage.c_str());
    } else {
        if (!matches.empty()) {
            ImGui::Text("%d/%d", currentMatchIdx + 1, (int)matches.size());
        } else {
            ImGui::Text("0/0");
        }
    }

    if (flags & SearchUIFlags::CASE_SENSITIVE) {
        ImGui::SameLine();
        if (ImGui::Checkbox("Aa", &caseSensitive)) {
            changed = true;
            isDirty = true;
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Case Sensitive");
    }

    if (flags & SearchUIFlags::MATCH_WHOLE_WORD) {
        ImGui::SameLine();
        if (ImGui::Checkbox("Mw", &matchWholeWord)) {
            changed = true;
            isDirty = true;
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Match Whole Word");
    }

    if (flags & SearchUIFlags::REGEX) {
        ImGui::SameLine();
        if (ImGui::Checkbox(".*", &useRegex)) {
            changed = true;
            isDirty = true;
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Regular Expression");
    }

    if ((flags & SearchUIFlags::REPLACE) && showReplace) {
        ImGui::Dummy(ImVec2(0, 2));
        
        ImGui::SetNextItemWidth(150);
        ImGui::InputTextWithHint("##replace", "Replace with...", replaceBuffer, IM_ARRAYSIZE(replaceBuffer));
        ImGui::SameLine();
        
        ImGui::BeginDisabled(matches.empty() || currentMatchIdx == -1);
        if (ImGui::Button("Replace")) {
            if (onReplaceClick) {
                onReplaceClick();
            }
        }
        ImGui::EndDisabled();
    }

    return changed;
}