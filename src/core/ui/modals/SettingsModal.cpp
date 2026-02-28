#include "core/ui/modals/SettingsModal.hpp"
#include "core/logging/Logger.hpp"
#include "application/AppSettings.hpp"
#include "core/input/InputState.hpp"
#include <imgui/imgui.h>
#include "platform/components/Keys.hpp"
#include "core/input/Keybinds.hpp"
#include "platform/Platform.hpp"

bool SettingsModal::initialize(Logger* logger, InputState* inputs, Keybinds* keybinds, Platform* platform, AppSettings* settings) {
    if (initialized) return false;
    loggerPtr = logger;
    settingsPtr = settings;
    inputsPtr = inputs;
    keybindsPtr = keybinds;
    platformPtr = platform;
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

void SettingsModal::drawStylesPage() {
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::TextUnformatted("Styles");
    ImGui::TextDisabled("Developer styling controls (ImGui style).");
    ImGui::Separator();

    // Scrollable container
    ImGui::BeginChild("##styles_scroll", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Helper: begin a 2-column table for neat alignment
    auto beginSectionTable = [](const char* id) -> bool {
        if (!ImGui::BeginTable(id, 2,
            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX))
            return false;

        ImGui::TableSetupColumn("##label",  ImGuiTableColumnFlags_WidthFixed, 220.0f);
        ImGui::TableSetupColumn("##value",  ImGuiTableColumnFlags_WidthStretch, 1.0f);
        return true;
    };
    auto endSectionTable = []() { ImGui::EndTable(); };

    auto row = [](const char* label, auto widgetFn) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        widgetFn();
    };

    // ===== Global =====
    if (ImGui::CollapsingHeader("Global")) {
        ImGui::Indent();
        if (beginSectionTable("##style_global")) {
            row("Alpha", [&] { ImGui::SliderFloat("##Alpha", &style.Alpha, 0.1f, 1.0f, "%0.2f"); });
            row("Disabled Alpha", [&] { ImGui::SliderFloat("##DisabledAlpha", &style.DisabledAlpha, 0.1f, 1.0f, "%0.2f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Window =====
    if (ImGui::CollapsingHeader("Window")) {
        ImGui::Indent();
        if (beginSectionTable("##style_window")) {
            row("Window Padding", [&] { ImGui::SliderFloat2("##WindowPadding", &style.WindowPadding.x, 1.0f, 20.0f, "%0.1f"); });
            row("Window Rounding", [&] { ImGui::SliderFloat("##WindowRounding", &style.WindowRounding, 0.0f, 10.0f, "%0.1f"); });
            row("Window Border Size", [&] { ImGui::SliderFloat("##WindowBorderSize", &style.WindowBorderSize, 0.0f, 2.0f, "%0.2f"); });
            row("Border Hover Padding", [&] { ImGui::SliderFloat("##WindowBorderHoverPadding", &style.WindowBorderHoverPadding, 0.1f, 8.0f, "%0.1f"); });
            row("Window Min Size", [&] { ImGui::SliderFloat2("##WindowMinSize", &style.WindowMinSize.x, 120.0f, 400.0f, "%0.0f"); });
            row("Window Title Align", [&] { ImGui::SliderFloat2("##WindowTitleAlign", &style.WindowTitleAlign.x, 0.0f, 1.0f, "%0.2f"); });

            row("Menu Button Position", [&] {
                int pos = 0;
                if (style.WindowMenuButtonPosition == ImGuiDir_None) pos = 0;
                else if (style.WindowMenuButtonPosition == ImGuiDir_Left) pos = 1;
                else if (style.WindowMenuButtonPosition == ImGuiDir_Right) pos = 2;
                const char* items[] = { "None", "Left", "Right" };
                if (ImGui::Combo("##WindowMenuButtonPosition", &pos, items, IM_ARRAYSIZE(items))) {
                    if (pos == 0) style.WindowMenuButtonPosition = ImGuiDir_None;
                    else if (pos == 1) style.WindowMenuButtonPosition = ImGuiDir_Left;
                    else if (pos == 2) style.WindowMenuButtonPosition = ImGuiDir_Right;
                }
            });

            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Child & Popup =====
    if (ImGui::CollapsingHeader("Child & Popup")) {
        ImGui::Indent();
        if (beginSectionTable("##style_child_popup")) {
            row("Child Rounding", [&] { ImGui::SliderFloat("##ChildRounding", &style.ChildRounding, 0.0f, 10.0f, "%0.1f"); });
            row("Child Border Size", [&] { ImGui::SliderFloat("##ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%0.2f"); });
            row("Popup Rounding", [&] { ImGui::SliderFloat("##PopupRounding", &style.PopupRounding, 0.0f, 10.0f, "%0.1f"); });
            row("Popup Border Size", [&] { ImGui::SliderFloat("##PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%0.2f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Frame & Layout =====
    if (ImGui::CollapsingHeader("Frame & Layout")) {
        ImGui::Indent();
        if (beginSectionTable("##style_frame_layout")) {
            row("Frame Padding", [&] { ImGui::SliderFloat2("##FramePadding", &style.FramePadding.x, 2.0f, 16.0f, "%0.1f"); });
            row("Frame Rounding", [&] { ImGui::SliderFloat("##FrameRounding", &style.FrameRounding, 0.0f, 10.0f, "%0.1f"); });
            row("Frame Border Size", [&] { ImGui::SliderFloat("##FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%0.2f"); });

            row("Item Spacing", [&] { ImGui::SliderFloat2("##ItemSpacing", &style.ItemSpacing.x, 2.0f, 20.0f, "%0.1f"); });
            row("Item Inner Spacing", [&] { ImGui::SliderFloat2("##ItemInnerSpacing", &style.ItemInnerSpacing.x, 0.0f, 12.0f, "%0.1f"); });
            row("Cell Padding", [&] { ImGui::SliderFloat2("##CellPadding", &style.CellPadding.x, 1.0f, 12.0f, "%0.1f"); });
            row("Touch Extra Padding", [&] { ImGui::SliderFloat2("##TouchExtraPadding", &style.TouchExtraPadding.x, 0.0f, 10.0f, "%0.1f"); });

            row("Indent Spacing", [&] { ImGui::SliderFloat("##IndentSpacing", &style.IndentSpacing, 14.0f, 40.0f, "%0.0f"); });
            row("Columns Min Spacing", [&] { ImGui::SliderFloat("##ColumnsMinSpacing", &style.ColumnsMinSpacing, 2.0f, 30.0f, "%0.0f"); });

            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Scrollbar & Grabs =====
    if (ImGui::CollapsingHeader("Scrollbars & Grabs")) {
        ImGui::Indent();
        if (beginSectionTable("##style_scroll_grab")) {
            row("Scrollbar Size", [&] { ImGui::SliderFloat("##ScrollbarSize", &style.ScrollbarSize, 5.0f, 25.0f, "%0.0f"); });
            row("Scrollbar Rounding", [&] { ImGui::SliderFloat("##ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 15.0f, "%0.1f"); });
            row("Scrollbar Padding", [&] { ImGui::SliderFloat("##ScrollbarPadding", &style.ScrollbarPadding, 0.0f, 5.0f, "%0.2f"); });
            row("Grab Min Size", [&] { ImGui::SliderFloat("##GrabMinSize", &style.GrabMinSize, 4.0f, 30.0f, "%0.1f"); });
            row("Grab Rounding", [&] { ImGui::SliderFloat("##GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%0.1f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Tabs =====
    if (ImGui::CollapsingHeader("Tabs")) {
        ImGui::Indent();
        if (beginSectionTable("##style_tabs")) {
            row("Tab Rounding", [&] { ImGui::SliderFloat("##TabRounding", &style.TabRounding, 0.0f, 12.0f, "%0.1f"); });
            row("Tab Border Size", [&] { ImGui::SliderFloat("##TabBorderSize", &style.TabBorderSize, 0.0f, 2.0f, "%0.2f"); });
            row("Tab Min Width Base", [&] { ImGui::SliderFloat("##TabMinWidthBase", &style.TabMinWidthBase, 0.0f, 250.0f, "%0.0f"); });
            row("Tab Min Width Shrink", [&] { ImGui::SliderFloat("##TabMinWidthShrink", &style.TabMinWidthShrink, 0.0f, 250.0f, "%0.0f"); });
            row("Close Btn Min W (Selected)", [&] { ImGui::SliderFloat("##TabCloseSel", &style.TabCloseButtonMinWidthSelected, -1.0f, 120.0f, "%0.1f"); });
            row("Close Btn Min W (Unselected)", [&] { ImGui::SliderFloat("##TabCloseUnsel", &style.TabCloseButtonMinWidthUnselected, -1.0f, 120.0f, "%0.1f"); });
            row("TabBar Border Size", [&] { ImGui::SliderFloat("##TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 3.0f, "%0.2f"); });
            row("TabBar Overline Size", [&] { ImGui::SliderFloat("##TabBarOverlineSize", &style.TabBarOverlineSize, 0.0f, 4.0f, "%0.2f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Tables & Tree =====
    if (ImGui::CollapsingHeader("Tables & Tree")) {
        ImGui::Indent();
        if (beginSectionTable("##style_tables_tree")) {
            row("Table Header Angle", [&] { ImGui::SliderFloat("##TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, 50.0f, "%0.1f"); });
            row("Table Header Text Align", [&] { ImGui::SliderFloat2("##TableAngledHeadersTextAlign", &style.TableAngledHeadersTextAlign.x, 0.0f, 1.0f, "%0.2f"); });

            row("Tree Lines Mode", [&] {
                int mode = 0;
                if (style.TreeLinesFlags == ImGuiTreeNodeFlags_DrawLinesFull) mode = 1;
                else if (style.TreeLinesFlags == ImGuiTreeNodeFlags_DrawLinesToNodes) mode = 2;
                else mode = 0;

                const char* items[] = { "None", "Full", "To Nodes" };
                if (ImGui::Combo("##TreeLinesMode", &mode, items, IM_ARRAYSIZE(items))) {
                    if (mode == 0) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesNone;
                    else if (mode == 1) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesFull;
                    else if (mode == 2) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesToNodes;
                }

                if (style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesNone &&
                    style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesFull &&
                    style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesToNodes) {
                    style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesNone;
                }
            });

            row("Tree Lines Size", [&] { ImGui::SliderFloat("##TreeLinesSize", &style.TreeLinesSize, 0.0f, 5.0f, "%0.2f"); });
            row("Tree Lines Rounding", [&] { ImGui::SliderFloat("##TreeLinesRounding", &style.TreeLinesRounding, 0.0f, 8.0f, "%0.1f"); });

            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Drag & Drop =====
    if (ImGui::CollapsingHeader("Drag & Drop")) {
        ImGui::Indent();
        if (beginSectionTable("##style_dragdrop")) {
            row("Target Rounding", [&] { ImGui::SliderFloat("##DragDropTargetRounding", &style.DragDropTargetRounding, 0.0f, 12.0f, "%0.1f"); });
            row("Target Border Size", [&] { ImGui::SliderFloat("##DragDropTargetBorderSize", &style.DragDropTargetBorderSize, 0.0f, 3.0f, "%0.2f"); });
            row("Target Padding", [&] { ImGui::SliderFloat("##DragDropTargetPadding", &style.DragDropTargetPadding, 0.0f, 12.0f, "%0.1f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Text & Alignment =====
    if (ImGui::CollapsingHeader("Text & Alignment")) {
        ImGui::Indent();
        if (beginSectionTable("##style_text_align")) {
            row("Color Button Position", [&] {
                int dir = (style.ColorButtonPosition == ImGuiDir_Left) ? 0 : 1;
                const char* items[] = { "Left", "Right" };
                if (ImGui::Combo("##ColorButtonPosition", &dir, items, IM_ARRAYSIZE(items))) {
                    style.ColorButtonPosition = (dir == 0) ? ImGuiDir_Left : ImGuiDir_Right;
                }
            });

            row("Button Text Align", [&] { ImGui::SliderFloat2("##ButtonTextAlign", &style.ButtonTextAlign.x, 0.0f, 1.0f, "%0.2f"); });
            row("Selectable Text Align", [&] { ImGui::SliderFloat2("##SelectableTextAlign", &style.SelectableTextAlign.x, 0.0f, 1.0f, "%0.2f"); });

            row("Separator Text Border", [&] { ImGui::SliderFloat("##SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 3.0f, "%0.2f"); });
            row("Separator Text Align", [&] { ImGui::SliderFloat2("##SeparatorTextAlign", &style.SeparatorTextAlign.x, 0.0f, 1.0f, "%0.2f"); });
            row("Separator Text Padding", [&] { ImGui::SliderFloat2("##SeparatorTextPadding", &style.SeparatorTextPadding.x, 0.0f, 24.0f, "%0.1f"); });

            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Rendering =====
    if (ImGui::CollapsingHeader("Rendering")) {
        ImGui::Indent();
        if (beginSectionTable("##style_rendering")) {
            row("AntiAliased Lines", [&] { ImGui::Checkbox("##AntiAliasedLines", &style.AntiAliasedLines); });
            row("AA Lines Use Tex", [&] { ImGui::Checkbox("##AntiAliasedLinesUseTex", &style.AntiAliasedLinesUseTex); });
            row("AntiAliased Fill", [&] { ImGui::Checkbox("##AntiAliasedFill", &style.AntiAliasedFill); });

            row("Curve Tessellation Tol", [&] { ImGui::SliderFloat("##CurveTessellationTol", &style.CurveTessellationTol, 0.5f, 8.0f, "%0.2f"); });
            row("Circle Tessellation Error", [&] { ImGui::SliderFloat("##CircleTessellationMaxError", &style.CircleTessellationMaxError, 0.5f, 8.0f, "%0.2f"); });

            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Misc =====
    if (ImGui::CollapsingHeader("Misc")) {
        ImGui::Indent();
        if (beginSectionTable("##style_misc")) {
            row("Log Slider Deadzone", [&] { ImGui::SliderFloat("##LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%0.1f"); });
            row("Image Border Size", [&] { ImGui::SliderFloat("##ImageBorderSize", &style.ImageBorderSize, 0.0f, 4.0f, "%0.1f"); });
            row("Display Window Padding", [&] { ImGui::SliderFloat2("##DisplayWindowPadding", &style.DisplayWindowPadding.x, 0.0f, 40.0f, "%0.0f"); });
            row("Display Safe Area Padding", [&] { ImGui::SliderFloat2("##DisplaySafeAreaPadding", &style.DisplaySafeAreaPadding.x, 0.0f, 40.0f, "%0.0f"); });
            row("Mouse Cursor Scale", [&] { ImGui::SliderFloat("##MouseCursorScale", &style.MouseCursorScale, 0.5f, 3.0f, "%0.2f"); });
            endSectionTable();
        }
        ImGui::Unindent();
        ImGui::Separator();
    }

    // ===== Colors =====
    static const char* kColorNames[ImGuiCol_COUNT] = {
        "Text","TextDisabled","WindowBg","ChildBg","PopupBg","Border","BorderShadow","FrameBg","FrameBgHovered","FrameBgActive",
        "TitleBg","TitleBgActive","TitleBgCollapsed","MenuBarBg","ScrollbarBg","ScrollbarGrab","ScrollbarGrabHovered","ScrollbarGrabActive",
        "CheckMark","SliderGrab","SliderGrabActive","Button","ButtonHovered","ButtonActive","Header","HeaderHovered","HeaderActive",
        "Separator","SeparatorHovered","SeparatorActive","ResizeGrip","ResizeGripHovered","ResizeGripActive","InputTextCursor","TabHovered",
        "Tab","TabSelected","TabSelectedOverline","TabDimmed","TabDimmedSelected","TabDimmedSelectedOverline","PlotLines","PlotLinesHovered",
        "PlotHistogram","PlotHistogramHovered","TableHeaderBg","TableBorderStrong","TableBorderLight","TableRowBg","TableRowBgAlt","TextLink",
        "TextSelectedBg","TreeLines","DragDropTarget","DragDropTargetBg","UnsavedMarker","NavCursor","NavWindowingHighlight",
        "NavWindowingDimBg","ModalWindowDimBg"
    };

    if (ImGui::CollapsingHeader("Application Colors")) {
        ImGui::Indent();
        if (beginSectionTable("##style_colors")) {
            row("Style Color", [&] { ImGui::Combo("##StyleColor", &selectedStyleColor, kColorNames, ImGuiCol_COUNT); });
            endSectionTable();
        }
        ImGui::Spacing();

        const float pickerWidth = 240.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float x = ImGui::GetCursorPosX() + (avail - pickerWidth) * 0.5f;
        if (x > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(x);

        ImGui::PushItemWidth(pickerWidth);
        ImGui::ColorPicker4("##Picker", &style.Colors[selectedStyleColor].x,
            ImGuiColorEditFlags_NoSmallPreview |
            ImGuiColorEditFlags_PickerHueBar
        );
        ImGui::PopItemWidth();
        ImGui::Unindent();
    }

    // ===== Editor Colors =====
    static const char* kEditorPaletteNames[] = {
        "Default", "Keyword", "Number", "String", "CharLiteral", "Punctuation", "Preprocessor", "Identifier", "Function", "KnownIdentifier",
        "PreprocIdentifier", "Comment", "MultiLineComment", "Background", "Cursor", "Selection", "ErrorMarker", "Breakpoint", "LineNumber",
        "CurrentLineFill", "CurrentLineFillInactive", "CurrentLineEdge"
    };

    if (ImGui::CollapsingHeader("Editor Colors")) {
        ImGui::Indent();
        if (beginSectionTable("##style_editorcolors")) {
            row("Style Color", [&] { ImGui::Combo("##StyleColor", &selectedEditorStyleColor, kEditorPaletteNames, (int)TextEditor::PaletteIndex::Max); });
            endSectionTable();
        }
        ImGui::Spacing();

        const float pickerWidth = 240.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float x = ImGui::GetCursorPosX() + (avail - pickerWidth) * 0.5f;
        if (x > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(x);

        ImGui::PushItemWidth(pickerWidth);
        if (ImGui::ColorPicker4("##Picker", &settingsPtr->styles.editorPalette[selectedEditorStyleColor].x,
            ImGuiColorEditFlags_NoSmallPreview |
            ImGuiColorEditFlags_PickerHueBar
        )) {
            settingsPtr->styles.paletteVersion++;
        }
        ImGui::PopItemWidth();
        ImGui::Unindent();
    }

    ImGui::EndChild();
}

void SettingsModal::drawGraphicsPage() {
    ImGui::TextUnformatted("Graphics");
    ImGui::Separator();

    if (ImGui::Checkbox("VSync", &settingsPtr->vsyncEnabled)) {
        platformPtr->swapInterval(settingsPtr->vsyncEnabled ? 1 : 0);
    }

    ImGui::TextDisabled("Prevents tearing but caps FPS to monitor refresh rate.");
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
        const bool selectedKeybinds = (page == SettingsPage::Keybinds);
        if (ImGui::Selectable("Keybinds", selectedKeybinds)) {
            page = SettingsPage::Keybinds;
        }

        const bool selectedStyles = (page == SettingsPage::Styles);
        if (ImGui::Selectable("Styles", selectedStyles)) {
            page = SettingsPage::Styles;
        }

        const bool selectedGraphics = (page == SettingsPage::Graphics);
        if (ImGui::Selectable("Graphics", selectedGraphics)) {
            page = SettingsPage::Graphics;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right pane
    ImGui::BeginChild("##settings_content", ImVec2(0, 0), true);
    {
        if (page == SettingsPage::Keybinds) {
            drawKeybindsPage();
        } else if (page == SettingsPage::Styles) {
            drawStylesPage();
        } else if (page == SettingsPage::Graphics) {
            drawGraphicsPage();
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
