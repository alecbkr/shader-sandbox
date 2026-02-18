#include "core/ui/modals/SettingsModal.hpp"
#include "core/logging/Logger.hpp"
#include "application/AppSettings.hpp"
#include "core/input/InputState.hpp"
#include <imgui/imgui.h>
#include "platform/components/Keys.hpp"
#include "core/input/Keybinds.hpp"

bool SettingsModal::initialize(Logger* logger, InputState* inputs, Keybinds* keybinds, AppSettings* settings) {
    if (initialized) return false;
    loggerPtr = logger;
    settingsPtr = settings;
    inputsPtr = inputs;
    keybindsPtr = keybinds;
    initialized = true;
    return true;
}

void SettingsModal::syncFromSettings() {
    if (!settingsPtr) return;
    
}

void SettingsModal::applyToSettings() {
    if (!settingsPtr) return;
    
}

void SettingsModal::updateCaptureFromInput() {
    if (!inputsPtr) return;

    // Cancel / Apply shortcuts
    if (inputsPtr->wasPressed(Key::Escape)) { capture = {}; return; }
    if (inputsPtr->wasPressed(Key::Enter)) {
        auto it = settingsPtr->keybindsMap.find(capture.bindingName);
        if (it != settingsPtr->keybindsMap.end()) {
            it->second.keys = capture.keysDraft;
        }
        capture = {};
        return;
    }
    if (inputsPtr->wasPressed(Key::Backspace)) {
        if (!capture.keysDraft.empty()) capture.keysDraft.pop_back();
    }

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) return;

    // Add new pressed keys this frame
    for (Key k : inputsPtr->pressedKeys) {
        if (k == Key::Unknown) continue;

        const u16 code = static_cast<u16>(k);

        bool exists = false;
        for (u16 existing : capture.keysDraft) {
            if (existing == code) { exists = true; break; }
        }
        if (!exists) capture.keysDraft.push_back(code);
    }
}


std::string SettingsModal::formatKeys(const std::vector<u16>& keys) const {
    if (keys.empty()) return "<none>";

    std::string out;
    for (size_t i = 0; i < keys.size(); i++) {
        if (i) out += " + ";
        out += keyCodeToString(keys[i]);
    }
    return out;
}

void SettingsModal::drawKeybindsPage() {
    ImGui::TextUnformatted("Keybinds");
    ImGui::Separator();

    // optional: filter
    static char filter[64] = "";
    ImGui::InputTextWithHint("##filter", "Filter actions...", filter, sizeof(filter));
    ImGui::Spacing();

    if (ImGui::BeginTable("##keybind_table", 3,
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableHeadersRow();

        for (auto& [name, bind] : settingsPtr->keybindsMap) {
            if (filter[0] != '\0' && name.find(filter) == std::string::npos)
                continue;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(name.c_str());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted(formatKeys(bind.keys).c_str());

            ImGui::TableNextColumn();
            ImGui::PushID(name.c_str());

            const bool isCapturingThis = capture.active && capture.bindingName == name;

            if (!isCapturingThis) {
                if (ImGui::Button("Rebind")) {
                    capture.active = true;
                    capture.bindingName = name;
                    capture.keysDraft = bind.keys;
                    inputsPtr->pressedKeys.clear();
                }
            } else {
                ImGui::TextUnformatted("Press keys...");
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    // Capture overlay section (only when capturing)
    if (capture.active) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Rebinding: %s", capture.bindingName.c_str());
        auto it = settingsPtr->keybindsMap.find(capture.bindingName);
        if (it != settingsPtr->keybindsMap.end()) {
            ImGui::Text("Current: %s", formatKeys(it->second.keys).c_str());
        }
        // capture input each frame while active
        updateCaptureFromInput();

        ImGui::Text("New:     %s", formatKeys(capture.keysDraft).c_str());

        ImGui::Spacing();

        if (ImGui::Button("Apply")) {
            auto it = settingsPtr->keybindsMap.find(capture.bindingName);
            if (it != settingsPtr->keybindsMap.end()) {
                it->second.keys = capture.keysDraft;
            }
            capture = {};
            keybindsPtr->syncBindings(settingsPtr->keybindsMap);
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            capture = {};
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            capture.keysDraft.clear();
        }

        ImGui::SameLine();
        ImGui::TextDisabled("Enter=Apply  Esc=Cancel");
    }
}


void SettingsModal::draw() {
    // ----- Modal body sizing -----
    ImGui::SetNextItemWidth(-1);

    // two-column layout (left nav + content)
    ImGui::BeginChild("##settings_root", ImVec2(900, 520), false);

    // Left pane
    ImGui::BeginChild("##settings_nav", ImVec2(180, 0), true);
    {
        ImGui::TextUnformatted("Settings");
        ImGui::Separator();

        // Page buttons (only keybinds for now)
        const bool selected = (page == SettingsPage::Keybinds);
        if (ImGui::Selectable("Keybinds", selected)) {
            page = SettingsPage::Keybinds;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right pane
    ImGui::BeginChild("##settings_content", ImVec2(0, 0), true);
    {
        if (page == SettingsPage::Keybinds) {
            drawKeybindsPage();
        }
    }
    ImGui::EndChild();

    ImGui::EndChild();

    // Footer
    ImGui::Separator();
    if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
    }
}
