#pragma once
#include <array>
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>
#include <types.hpp>
#include "core/ui/TextEditor.h"

using json = nlohmann::json;

struct SettingsStyles {
    bool hasLoadedStyles = false;
    bool hasLoadedPalette = false;
    u32 paletteVersion = 0;

    // ---- Core ----
    float alpha = 1.0f;
    float disabledAlpha = 0.60f;

    // ---- Window ----
    ImVec2 windowPadding = ImVec2(8.0f, 8.0f);
    float  windowRounding = 0.0f;
    float  windowBorderSize = 1.0f;
    float  windowBorderHoverPadding = 4.0f;
    ImVec2 windowMinSize = ImVec2(32.0f, 32.0f);
    ImVec2 windowTitleAlign = ImVec2(0.0f, 0.5f);
    ImGuiDir windowMenuButtonPosition = ImGuiDir_Left;

    // ---- Child/Popup ----
    float childRounding = 0.0f;
    float childBorderSize = 1.0f;
    float popupRounding = 0.0f;
    float popupBorderSize = 1.0f;

    // ---- Frame ----
    ImVec2 framePadding = ImVec2(4.0f, 3.0f);
    float  frameRounding = 0.0f;
    float  frameBorderSize = 0.0f;

    // ---- Layout ----
    ImVec2 itemSpacing = ImVec2(8.0f, 4.0f);
    ImVec2 itemInnerSpacing = ImVec2(4.0f, 4.0f);
    ImVec2 cellPadding = ImVec2(4.0f, 2.0f);
    ImVec2 touchExtraPadding = ImVec2(0.0f, 0.0f);
    float  indentSpacing = 21.0f;
    float  columnsMinSpacing = 6.0f;

    // ---- Scrollbar/Grab ----
    float scrollbarSize = 14.0f;
    float scrollbarRounding = 9.0f;
    float scrollbarPadding = 2.0f;
    float grabMinSize = 10.0f;
    float grabRounding = 0.0f;

    // ---- Misc ----
    float logSliderDeadzone = 4.0f;
    float imageBorderSize = 0.0f;

    // ---- Tabs ----
    float tabRounding = 4.0f;
    float tabBorderSize = 0.0f;
    float tabMinWidthBase = 20.0f;
    float tabMinWidthShrink = 0.0f;
    float tabCloseButtonMinWidthSelected = 0.0f;
    float tabCloseButtonMinWidthUnselected = 0.0f;
    float tabBarBorderSize = 1.0f;
    float tabBarOverlineSize = 2.0f;

    // ---- Tables ----
    float tableAngledHeadersAngle = 35.0f;
    ImVec2 tableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

    // ---- Tree lines ----
    ImGuiTreeNodeFlags treeLinesFlags = ImGuiTreeNodeFlags_DrawLinesNone;
    float treeLinesSize = 0.0f;
    float treeLinesRounding = 0.0f;

    // ---- Drag/Drop ----
    float dragDropTargetRounding = 0.0f;
    float dragDropTargetBorderSize = 0.0f;
    float dragDropTargetPadding = 0.0f;

    // ---- Buttons/Selectable ----
    ImGuiDir colorButtonPosition = ImGuiDir_Right;
    ImVec2 buttonTextAlign = ImVec2(0.5f, 0.5f);
    ImVec2 selectableTextAlign = ImVec2(0.0f, 0.0f);

    // ---- Separator text ----
    float separatorTextBorderSize = 0.0f;
    ImVec2 separatorTextAlign = ImVec2(0.0f, 0.5f);
    ImVec2 separatorTextPadding = ImVec2(20.0f, 3.0f);

    // ---- Display ----
    ImVec2 displayWindowPadding = ImVec2(19.0f, 19.0f);
    ImVec2 displaySafeAreaPadding = ImVec2(3.0f, 3.0f);
    float mouseCursorScale = 1.0f;

    // ---- Rendering ----
    bool antiAliasedLines = true;
    bool antiAliasedLinesUseTex = true;
    bool antiAliasedFill = true;
    float curveTessellationTol = 1.25f;
    float circleTessellationMaxError = 0.30f;

    // ---- Colors ----
    std::array<ImVec4, ImGuiCol_COUNT> colors = {};

    // UI helper (not really style, but tied to style page UX)
    int selectedStyleColor = ImGuiCol_Text;

    static constexpr int EditorPaletteCount = (int)TextEditor::PaletteIndex::Max;
    std::array<ImVec4, EditorPaletteCount> editorPalette = {};

    // UI helper for the settings page
    int selectedEditorPaletteColor = 0;

    // ---- Apply / Capture ----
    void applyToImGui(ImGuiStyle& s) const {
        s.Alpha = alpha;
        s.DisabledAlpha = disabledAlpha;

        s.WindowPadding = windowPadding;
        s.WindowRounding = windowRounding;
        s.WindowBorderSize = windowBorderSize;
        s.WindowBorderHoverPadding = windowBorderHoverPadding;
        s.WindowMinSize = windowMinSize;
        s.WindowTitleAlign = windowTitleAlign;
        s.WindowMenuButtonPosition = windowMenuButtonPosition;

        s.ChildRounding = childRounding;
        s.ChildBorderSize = childBorderSize;
        s.PopupRounding = popupRounding;
        s.PopupBorderSize = popupBorderSize;

        s.FramePadding = framePadding;
        s.FrameRounding = frameRounding;
        s.FrameBorderSize = frameBorderSize;

        s.ItemSpacing = itemSpacing;
        s.ItemInnerSpacing = itemInnerSpacing;
        s.CellPadding = cellPadding;
        s.TouchExtraPadding = touchExtraPadding;
        s.IndentSpacing = indentSpacing;
        s.ColumnsMinSpacing = columnsMinSpacing;

        s.ScrollbarSize = scrollbarSize;
        s.ScrollbarRounding = scrollbarRounding;
        s.ScrollbarPadding = scrollbarPadding;
        s.GrabMinSize = grabMinSize;
        s.GrabRounding = grabRounding;

        s.LogSliderDeadzone = logSliderDeadzone;
        s.ImageBorderSize = imageBorderSize;

        s.TabRounding = tabRounding;
        s.TabBorderSize = tabBorderSize;
        s.TabMinWidthBase = tabMinWidthBase;
        s.TabMinWidthShrink = tabMinWidthShrink;
        s.TabCloseButtonMinWidthSelected = tabCloseButtonMinWidthSelected;
        s.TabCloseButtonMinWidthUnselected = tabCloseButtonMinWidthUnselected;
        s.TabBarBorderSize = tabBarBorderSize;
        s.TabBarOverlineSize = tabBarOverlineSize;

        s.TableAngledHeadersAngle = tableAngledHeadersAngle;
        s.TableAngledHeadersTextAlign = tableAngledHeadersTextAlign;

        s.TreeLinesFlags = treeLinesFlags;
        s.TreeLinesSize = treeLinesSize;
        s.TreeLinesRounding = treeLinesRounding;

        s.DragDropTargetRounding = dragDropTargetRounding;
        s.DragDropTargetBorderSize = dragDropTargetBorderSize;
        s.DragDropTargetPadding = dragDropTargetPadding;

        s.ColorButtonPosition = colorButtonPosition;
        s.ButtonTextAlign = buttonTextAlign;
        s.SelectableTextAlign = selectableTextAlign;

        s.SeparatorTextBorderSize = separatorTextBorderSize;
        s.SeparatorTextAlign = separatorTextAlign;
        s.SeparatorTextPadding = separatorTextPadding;

        s.DisplayWindowPadding = displayWindowPadding;
        s.DisplaySafeAreaPadding = displaySafeAreaPadding;
        s.MouseCursorScale = mouseCursorScale;

        s.AntiAliasedLines = antiAliasedLines;
        s.AntiAliasedLinesUseTex = antiAliasedLinesUseTex;
        s.AntiAliasedFill = antiAliasedFill;

        s.CurveTessellationTol = curveTessellationTol;
        s.CircleTessellationMaxError = circleTessellationMaxError;

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            s.Colors[i] = colors[(size_t)i];
        }
    }

    void captureFromImGui(const ImGuiStyle& s) {
        alpha = s.Alpha;
        disabledAlpha = s.DisabledAlpha;

        windowPadding = s.WindowPadding;
        windowRounding = s.WindowRounding;
        windowBorderSize = s.WindowBorderSize;
        windowBorderHoverPadding = s.WindowBorderHoverPadding;
        windowMinSize = s.WindowMinSize;
        windowTitleAlign = s.WindowTitleAlign;
        windowMenuButtonPosition = s.WindowMenuButtonPosition;

        childRounding = s.ChildRounding;
        childBorderSize = s.ChildBorderSize;
        popupRounding = s.PopupRounding;
        popupBorderSize = s.PopupBorderSize;

        framePadding = s.FramePadding;
        frameRounding = s.FrameRounding;
        frameBorderSize = s.FrameBorderSize;

        itemSpacing = s.ItemSpacing;
        itemInnerSpacing = s.ItemInnerSpacing;
        cellPadding = s.CellPadding;
        touchExtraPadding = s.TouchExtraPadding;
        indentSpacing = s.IndentSpacing;
        columnsMinSpacing = s.ColumnsMinSpacing;

        scrollbarSize = s.ScrollbarSize;
        scrollbarRounding = s.ScrollbarRounding;
        scrollbarPadding = s.ScrollbarPadding;
        grabMinSize = s.GrabMinSize;
        grabRounding = s.GrabRounding;

        logSliderDeadzone = s.LogSliderDeadzone;
        imageBorderSize = s.ImageBorderSize;

        tabRounding = s.TabRounding;
        tabBorderSize = s.TabBorderSize;
        tabMinWidthBase = s.TabMinWidthBase;
        tabMinWidthShrink = s.TabMinWidthShrink;
        tabCloseButtonMinWidthSelected = s.TabCloseButtonMinWidthSelected;
        tabCloseButtonMinWidthUnselected = s.TabCloseButtonMinWidthUnselected;
        tabBarBorderSize = s.TabBarBorderSize;
        tabBarOverlineSize = s.TabBarOverlineSize;

        tableAngledHeadersAngle = s.TableAngledHeadersAngle;
        tableAngledHeadersTextAlign = s.TableAngledHeadersTextAlign;

        treeLinesFlags = s.TreeLinesFlags;
        treeLinesSize = s.TreeLinesSize;
        treeLinesRounding = s.TreeLinesRounding;

        dragDropTargetRounding = s.DragDropTargetRounding;
        dragDropTargetBorderSize = s.DragDropTargetBorderSize;
        dragDropTargetPadding = s.DragDropTargetPadding;

        colorButtonPosition = s.ColorButtonPosition;
        buttonTextAlign = s.ButtonTextAlign;
        selectableTextAlign = s.SelectableTextAlign;

        separatorTextBorderSize = s.SeparatorTextBorderSize;
        separatorTextAlign = s.SeparatorTextAlign;
        separatorTextPadding = s.SeparatorTextPadding;

        displayWindowPadding = s.DisplayWindowPadding;
        displaySafeAreaPadding = s.DisplaySafeAreaPadding;
        mouseCursorScale = s.MouseCursorScale;

        antiAliasedLines = s.AntiAliasedLines;
        antiAliasedLinesUseTex = s.AntiAliasedLinesUseTex;
        antiAliasedFill = s.AntiAliasedFill;

        curveTessellationTol = s.CurveTessellationTol;
        circleTessellationMaxError = s.CircleTessellationMaxError;

        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            colors[(size_t)i] = s.Colors[i];
        }
    }

    void saveStyles(json& j) const {
        json s;

        s["alpha"] = alpha;
        s["disabledAlpha"] = disabledAlpha;

        s["windowPadding"] = { windowPadding.x, windowPadding.y };
        s["windowRounding"] = windowRounding;
        s["windowBorderSize"] = windowBorderSize;
        s["windowBorderHoverPadding"] = windowBorderHoverPadding;
        s["windowMinSize"] = { windowMinSize.x, windowMinSize.y };
        s["windowTitleAlign"] = { windowTitleAlign.x, windowTitleAlign.y };
        s["windowMenuButtonPosition"] = (int)windowMenuButtonPosition;

        s["childRounding"] = childRounding;
        s["childBorderSize"] = childBorderSize;
        s["popupRounding"] = popupRounding;
        s["popupBorderSize"] = popupBorderSize;

        s["framePadding"] = { framePadding.x, framePadding.y };
        s["frameRounding"] = frameRounding;
        s["frameBorderSize"] = frameBorderSize;

        s["itemSpacing"] = { itemSpacing.x, itemSpacing.y };
        s["itemInnerSpacing"] = { itemInnerSpacing.x, itemInnerSpacing.y };
        s["cellPadding"] = { cellPadding.x, cellPadding.y };
        s["touchExtraPadding"] = { touchExtraPadding.x, touchExtraPadding.y };
        s["indentSpacing"] = indentSpacing;
        s["columnsMinSpacing"] = columnsMinSpacing;

        s["scrollbarSize"] = scrollbarSize;
        s["scrollbarRounding"] = scrollbarRounding;
        s["scrollbarPadding"] = scrollbarPadding;
        s["grabMinSize"] = grabMinSize;
        s["grabRounding"] = grabRounding;

        s["logSliderDeadzone"] = logSliderDeadzone;
        s["imageBorderSize"] = imageBorderSize;

        s["tabRounding"] = tabRounding;
        s["tabBorderSize"] = tabBorderSize;
        s["tabMinWidthBase"] = tabMinWidthBase;
        s["tabMinWidthShrink"] = tabMinWidthShrink;
        s["tabCloseButtonMinWidthSelected"] = tabCloseButtonMinWidthSelected;
        s["tabCloseButtonMinWidthUnselected"] = tabCloseButtonMinWidthUnselected;
        s["tabBarBorderSize"] = tabBarBorderSize;
        s["tabBarOverlineSize"] = tabBarOverlineSize;

        s["tableAngledHeadersAngle"] = tableAngledHeadersAngle;
        s["tableAngledHeadersTextAlign"] = { tableAngledHeadersTextAlign.x, tableAngledHeadersTextAlign.y };

        s["treeLinesFlags"] = (int)treeLinesFlags;
        s["treeLinesSize"] = treeLinesSize;
        s["treeLinesRounding"] = treeLinesRounding;

        s["dragDropTargetRounding"] = dragDropTargetRounding;
        s["dragDropTargetBorderSize"] = dragDropTargetBorderSize;
        s["dragDropTargetPadding"] = dragDropTargetPadding;

        s["colorButtonPosition"] = (int)colorButtonPosition;
        s["buttonTextAlign"] = { buttonTextAlign.x, buttonTextAlign.y };
        s["selectableTextAlign"] = { selectableTextAlign.x, selectableTextAlign.y };

        s["separatorTextBorderSize"] = separatorTextBorderSize;
        s["separatorTextAlign"] = { separatorTextAlign.x, separatorTextAlign.y };
        s["separatorTextPadding"] = { separatorTextPadding.x, separatorTextPadding.y };

        s["displayWindowPadding"] = { displayWindowPadding.x, displayWindowPadding.y };
        s["displaySafeAreaPadding"] = { displaySafeAreaPadding.x, displaySafeAreaPadding.y };
        s["mouseCursorScale"] = mouseCursorScale;

        s["antiAliasedLines"] = antiAliasedLines;
        s["antiAliasedLinesUseTex"] = antiAliasedLinesUseTex;
        s["antiAliasedFill"] = antiAliasedFill;

        s["curveTessellationTol"] = curveTessellationTol;
        s["circleTessellationMaxError"] = circleTessellationMaxError;

        // Colors
        json colorArray = json::array();
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            colorArray.push_back({
                colors[i].x,
                colors[i].y,
                colors[i].z,
                colors[i].w
            });
        }
        s["colors"] = colorArray;

        // Editor Palette
        json editorPaletteArray = json::array();
        for (int i = 0; i < EditorPaletteCount; i++) {
            editorPaletteArray.push_back({
                editorPalette[i].x,
                editorPalette[i].y,
                editorPalette[i].z,
                editorPalette[i].w
            });
        }
        s["editorPalette"] = editorPaletteArray;

        j["styles"] = s;
    }

    void loadStyles(const json& j) {
        hasLoadedStyles = false;
        if (!j.contains("styles") || !j["styles"].is_object()) return;
        hasLoadedStyles = true;

        const json& s = j["styles"];

        alpha = s.value("alpha", alpha);
        disabledAlpha = s.value("disabledAlpha", disabledAlpha);

        if (s.contains("windowPadding"))
            windowPadding = ImVec2(s["windowPadding"][0], s["windowPadding"][1]);

        windowRounding = s.value("windowRounding", windowRounding);
        windowBorderSize = s.value("windowBorderSize", windowBorderSize);
        windowBorderHoverPadding = s.value("windowBorderHoverPadding", windowBorderHoverPadding);

        if (s.contains("windowMinSize"))
            windowMinSize = ImVec2(s["windowMinSize"][0], s["windowMinSize"][1]);

        if (s.contains("windowTitleAlign"))
            windowTitleAlign = ImVec2(s["windowTitleAlign"][0], s["windowTitleAlign"][1]);

        windowMenuButtonPosition = (ImGuiDir)s.value("windowMenuButtonPosition", (int)windowMenuButtonPosition);

        childRounding = s.value("childRounding", childRounding);
        childBorderSize = s.value("childBorderSize", childBorderSize);
        popupRounding = s.value("popupRounding", popupRounding);
        popupBorderSize = s.value("popupBorderSize", popupBorderSize);

        if (s.contains("framePadding"))
            framePadding = ImVec2(s["framePadding"][0], s["framePadding"][1]);

        frameRounding = s.value("frameRounding", frameRounding);
        frameBorderSize = s.value("frameBorderSize", frameBorderSize);

        if (s.contains("colors") && s["colors"].is_array()) {
            for (size_t i = 0; i < s["colors"].size() && i < ImGuiCol_COUNT; i++) {
                const auto& c = s["colors"][i];
                colors[i] = ImVec4(c[0], c[1], c[2], c[3]);
            }
        }

        hasLoadedPalette = false;
        if (!s.contains("editorPalette") || !s["editorPalette"].is_array()) return;
        hasLoadedPalette = true;

        if (s.contains("editorPalette") && s["editorPalette"].is_array()) {
            for (size_t i = 0; i < s["editorPalette"].size() && i < (size_t)EditorPaletteCount; i++) {
                const auto& c = s["editorPalette"][i];
                editorPalette[i] = ImVec4(c[0], c[1], c[2], c[3]);
            }
        }
    }
};
