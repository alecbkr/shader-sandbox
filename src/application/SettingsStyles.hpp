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

    // Inspector Specific Styles
    ImVec4 inspectorBorderColor;
    ImVec4 inspectorTabBackgroundColor;
    ImVec4 inspectorTitleBackgroundColor;
    ImVec4 inspectorBodyColor;

    float inspectorBorderThickness = 1.0f;
    float inspectorBodyPadding = 12.0f;
    float inspectorTitleInnerPadding = 1.0f;
    float inspectorBodyRounding = 6.0f;
    float inspectorTitleOffset = 6.0f;

    // General inspector tree/child hover color
    ImVec4 inspectorTreeHoveredColor;

    // Materials tab specific styles
    // Materials tab colors
    ImVec4 materialsBorderColor;
    ImVec4 materialsTabBackgroundColor;
    ImVec4 materialsTitleBackgroundColor;
    ImVec4 materialsTreeBodyColor;

    // Materials tab styles
    float materialsBorderThickness = 1.0f;
    float materialsBodyPadding = 12.0f;
    float materialsTitleInnerPadding = 1.0f;
    float materialsBodyRounding = 6.0f;
    float materialsTitleOffset = 6.0f;

    // Assets tab specific styles
    // Assest tab colors
    ImVec4 assetsDirectoryTextColor;
    ImVec4 assetsFileBackgroundColor;
    ImVec4 assetsBorderColor;
    ImVec4 assetsTabBackgroundColor;
    ImVec4 assetsTitleBackgroundColor;
    ImVec4 assetsTreeBodyColor;

    // Assets tab styles
    float assetsBorderThickness = 1.0f;
    float assetsBodyPadding = 12.0f;
    float assetsTitleInnerPadding = 1.0f;
    float assetsBodyRounding = 6.0f;
    float assetsTitleOffset = 6.0f;

    // Console Specific Styles 
    // Log Category Colors
    ImVec4 consoleCriticalColor        = ImVec4(1.00000f, 0.33333f, 0.33333f, 1.00f); // Red (#ff5555)
    ImVec4 consoleErrorColor           = ImVec4(1.00000f, 0.47451f, 0.77647f, 1.00f); // Pink (#ff79c6)
    ImVec4 consoleWarningColor         = ImVec4(1.00000f, 0.72157f, 0.42353f, 1.00f); // Orange (#ffb86c)
    ImVec4 consoleInfoColor            = ImVec4(0.31373f, 0.98039f, 0.48235f, 1.00f); // Green (#50fa7b)
    
    // UI Element Colors
    ImVec4 consoleWindowBgColor        = ImVec4(0.15686f, 0.16471f, 0.21176f, 1.00f); // Default Background
    ImVec4 consoleMenuBarBgColor       = ImVec4(0.12157f, 0.12549f, 0.16471f, 1.00f); // Darker Background (#1f202a)
    ImVec4 consoleTextSelectedBgColor  = ImVec4(0.26667f, 0.27843f, 0.35294f, 0.80f); // Selection (#44475a)
    ImVec4 consoleSearchHighlightColor = ImVec4(0.74118f, 0.57647f, 0.97647f, 0.50f); // Purple Accent (#bd93f9)
    // Shader tab colors
    ImVec4 shaderTabBackgroundColor;
    ImVec4 shaderTitleBackgroundColor;
    ImVec4 shaderTreeBodyColor;
    ImVec4 shaderBorderColor;

    float shaderBorderThickness = 1.0f;
    float shaderBodyPadding = 12.0f;
    float shaderTitleInnerPadding = 1.0f;
    float shaderBodyRounding = 6.0f;
    float shaderTitleOffset = 6.0f;

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

        // Inspector Specific Styles
        json inspectorStylesObject = json::object();

        inspectorStylesObject["inspectorBorderColor"] = {
            inspectorBorderColor.x,
            inspectorBorderColor.y,
            inspectorBorderColor.z,
            inspectorBorderColor.w
        };
        inspectorStylesObject["inspectorTabBackgroundColor"] = {
            inspectorTabBackgroundColor.x,
            inspectorTabBackgroundColor.y,
            inspectorTabBackgroundColor.z,
            inspectorTabBackgroundColor.w
        };
        inspectorStylesObject["inspectorTitleBackgroundColor"] = {
            inspectorTitleBackgroundColor.x,
            inspectorTitleBackgroundColor.y,
            inspectorTitleBackgroundColor.z,
            inspectorTitleBackgroundColor.w
        };
        inspectorStylesObject["inspectorBodyColor"] = {
            inspectorBodyColor.x,
            inspectorBodyColor.y,
            inspectorBodyColor.z,
            inspectorBodyColor.w
        };

        inspectorStylesObject["inspectorBorderThickness"] = inspectorBorderThickness;
        inspectorStylesObject["inspectorBodyPadding"] = inspectorBodyPadding;
        inspectorStylesObject["inspectorTitleInnerPadding"] = inspectorTitleInnerPadding;
        inspectorStylesObject["inspectorBodyRounding"] = inspectorBodyRounding;
        inspectorStylesObject["inspectorTitleOffset"] = inspectorTitleOffset;

        // Materials tab styles
        json materialsTabObject = json::object();
        materialsTabObject["materialsBorderColor"] = {
            materialsBorderColor.x,
            materialsBorderColor.y,
            materialsBorderColor.z,
            materialsBorderColor.w
        };
        materialsTabObject["materialsTabBackgroundColor"] = {
            materialsTabBackgroundColor.x,
            materialsTabBackgroundColor.y,
            materialsTabBackgroundColor.z,
            materialsTabBackgroundColor.w
        };
        materialsTabObject["materialsTitleBackgroundColor"] = {
            materialsTitleBackgroundColor.x,
            materialsTitleBackgroundColor.y,
            materialsTitleBackgroundColor.z,
            materialsTitleBackgroundColor.w
        };
        materialsTabObject["materialsTreeBodyColor"] = {
            materialsTreeBodyColor.x,
            materialsTreeBodyColor.y,
            materialsTreeBodyColor.z,
            materialsTreeBodyColor.w
        };

        materialsTabObject["materialsBorderThickness"] = materialsBorderThickness;
        materialsTabObject["materialsBodyPadding"] = materialsBodyPadding;
        materialsTabObject["materialsTitleInnerPadding"] = materialsTitleInnerPadding;
        materialsTabObject["materialsBodyRounding"] = materialsBodyRounding;
        materialsTabObject["materialsTitleOffset"] = materialsTitleOffset;

        inspectorStylesObject["materials"] = materialsTabObject;

        // Assets tab styles
        json assetsTabObject = json::object();
        assetsTabObject["assetsDirectoryTextColor"] = {
            assetsDirectoryTextColor.x,
            assetsDirectoryTextColor.y,
            assetsDirectoryTextColor.z,
            assetsDirectoryTextColor.w
        };
        assetsTabObject["assetsFileBackgroundColor"] = {
            assetsFileBackgroundColor.x,
            assetsFileBackgroundColor.y,
            assetsFileBackgroundColor.z,
            assetsFileBackgroundColor.w
        };
        assetsTabObject["assetsBorderColor"] = {
            assetsBorderColor.x,
            assetsBorderColor.y,
            assetsBorderColor.z,
            assetsBorderColor.w
        };
        assetsTabObject["assetsTabBackgroundColor"] = {
            assetsTabBackgroundColor.x,
            assetsTabBackgroundColor.y,
            assetsTabBackgroundColor.z,
            assetsTabBackgroundColor.w
        };
        assetsTabObject["assetsTitleBackgroundColor"] = {
            assetsTitleBackgroundColor.x,
            assetsTitleBackgroundColor.y,
            assetsTitleBackgroundColor.z,
            assetsTitleBackgroundColor.w
        };
        assetsTabObject["assetsTreeBodyColor"] = {
            assetsTreeBodyColor.x,
            assetsTreeBodyColor.y,
            assetsTreeBodyColor.z,
            assetsTreeBodyColor.w
        };

        assetsTabObject["assetsBorderThickness"] = assetsBorderThickness;
        assetsTabObject["assetsBodyPadding"] = assetsBodyPadding;
        assetsTabObject["assetsTitleInnerPadding"] = assetsTitleInnerPadding;
        assetsTabObject["assetsBodyRounding"] = assetsBodyRounding;
        assetsTabObject["assetsTitleOffset"] = assetsTitleOffset;

        inspectorStylesObject["assets"] = assetsTabObject;

        json shaderTabObject = json::object();

        // Shaders tab styles
        shaderTabObject["shaderTabBackgroundColor"] = {
            shaderTabBackgroundColor.x,
            shaderTabBackgroundColor.y,
            shaderTabBackgroundColor.z,
            shaderTabBackgroundColor.w
        };
        shaderTabObject["shaderTitleBackgroundColor"] = {
            shaderTitleBackgroundColor.x,
            shaderTitleBackgroundColor.y,
            shaderTitleBackgroundColor.z,
            shaderTitleBackgroundColor.w
        };
        shaderTabObject["shaderTreeBodyColor"] = {
            shaderTreeBodyColor.x,
            shaderTreeBodyColor.y,
            shaderTreeBodyColor.z,
            shaderTreeBodyColor.w
        };
        shaderTabObject["shaderBorderColor"] = {
            shaderBorderColor.x,
            shaderBorderColor.y,
            shaderBorderColor.z,
            shaderBorderColor.w
        };

        shaderTabObject["shaderBorderThickness"] = shaderBorderThickness;
        shaderTabObject["shaderBodyPadding"] = shaderBodyPadding;
        shaderTabObject["shaderTitleInnerPadding"] = shaderTitleInnerPadding;
        shaderTabObject["shaderBodyRounding"] = shaderBodyRounding;
        shaderTabObject["shaderTitleOffset"] = shaderTitleOffset;

        inspectorStylesObject["shaders"] = shaderTabObject;

        s["inspector"] = inspectorStylesObject;

        // Console specific styles 
        json consoleStylesObject = json::object();
        consoleStylesObject["criticalColor"] = { consoleCriticalColor.x, consoleCriticalColor.y, consoleCriticalColor.z, consoleCriticalColor.w };
        consoleStylesObject["errorColor"]    = { consoleErrorColor.x, consoleErrorColor.y, consoleErrorColor.z, consoleErrorColor.w };
        consoleStylesObject["warningColor"]  = { consoleWarningColor.x, consoleWarningColor.y, consoleWarningColor.z, consoleWarningColor.w };
        consoleStylesObject["infoColor"]     = { consoleInfoColor.x, consoleInfoColor.y, consoleInfoColor.z, consoleInfoColor.w };
        consoleStylesObject["searchHighlightColor"] = { consoleSearchHighlightColor.x, consoleSearchHighlightColor.y, consoleSearchHighlightColor.z, consoleSearchHighlightColor.w };
        consoleStylesObject["windowBgColor"]       = { consoleWindowBgColor.x, consoleWindowBgColor.y, consoleWindowBgColor.z, consoleWindowBgColor.w };
        consoleStylesObject["menuBarBgColor"]      = { consoleMenuBarBgColor.x, consoleMenuBarBgColor.y, consoleMenuBarBgColor.z, consoleMenuBarBgColor.w };
        consoleStylesObject["textSelectedBgColor"] = { consoleTextSelectedBgColor.x, consoleTextSelectedBgColor.y, consoleTextSelectedBgColor.z, consoleTextSelectedBgColor.w };
        
        s["console"] = consoleStylesObject;


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

        if (s.contains("inspector") && s["inspector"].is_object()) {
            const json& ins = s["inspector"];

            // Accept both legacy typo key and correct key for compatibility.
            if (ins.contains("inspectorBorderColor") || ins.contains("inspectorBoderColor")) {
                const json& inspectorBorderColorNode = ins.contains("inspectorBorderColor")
                    ? ins["inspectorBorderColor"]
                    : ins["inspectorBoderColor"];
                inspectorBorderColor = ImVec4(
                    inspectorBorderColorNode[0],
                    inspectorBorderColorNode[1],
                    inspectorBorderColorNode[2],
                    inspectorBorderColorNode[3]
                );
            }
            if (ins.contains("inspectorTabBackgroundColor")) {   
                inspectorTabBackgroundColor = ImVec4(
                    ins["inspectorTabBackgroundColor"][0],
                    ins["inspectorTabBackgroundColor"][1],
                    ins["inspectorTabBackgroundColor"][2],
                    ins["inspectorTabBackgroundColor"][3]
                );
            }
            if (ins.contains("inspectorTitleBackgroundColor")) {
                inspectorTitleBackgroundColor = ImVec4(
                    ins["inspectorTitleBackgroundColor"][0],
                    ins["inspectorTitleBackgroundColor"][1],
                    ins["inspectorTitleBackgroundColor"][2],
                    ins["inspectorTitleBackgroundColor"][3]
                );
            }
            if (ins.contains("inspectorBodyColor")) {
                inspectorBodyColor = ImVec4(
                    ins["inspectorBodyColor"][0],
                    ins["inspectorBodyColor"][1],
                    ins["inspectorBodyColor"][2],
                    ins["inspectorBodyColor"][3]
                );
            }

            inspectorBorderThickness = ins.value("inspectorBorderThickness", inspectorBorderThickness);
            inspectorBodyPadding = ins.value("inspectorBodyPadding", inspectorBodyPadding);
            inspectorTitleInnerPadding = ins.value("inspectorTitleInnerPadding", inspectorTitleInnerPadding);
            inspectorBodyRounding = ins.value("inspectorBodyRounding", inspectorBodyRounding);
            inspectorTitleOffset = ins.value("inspectorTitleOffset", inspectorTitleOffset);

            if (ins.contains("materials") && ins["materials"].is_object()) {
                const json& mats = ins["materials"];
                materialsBorderColor = ImVec4(
                    mats["materialsBorderColor"][0],
                    mats["materialsBorderColor"][1],
                    mats["materialsBorderColor"][2],
                    mats["materialsBorderColor"][3]
                );
                materialsTabBackgroundColor = ImVec4(
                    mats["materialsTabBackgroundColor"][0],
                    mats["materialsTabBackgroundColor"][1],
                    mats["materialsTabBackgroundColor"][2],
                    mats["materialsTabBackgroundColor"][3]
                );
                materialsTitleBackgroundColor = ImVec4(
                    mats["materialsTitleBackgroundColor"][0],
                    mats["materialsTitleBackgroundColor"][1],
                    mats["materialsTitleBackgroundColor"][2],
                    mats["materialsTitleBackgroundColor"][3]
                );
                materialsTreeBodyColor = ImVec4(
                    mats["materialsTreeBodyColor"][0],
                    mats["materialsTreeBodyColor"][1],
                    mats["materialsTreeBodyColor"][2],
                    mats["materialsTreeBodyColor"][3]
                );
                materialsBorderThickness = mats.value("materialsBorderThickness", materialsBorderThickness);
                materialsBodyPadding = mats.value("materialsBodyPadding", materialsBodyPadding);
                materialsTitleInnerPadding = mats.value("materialsTitleInnerPadding", materialsTitleInnerPadding);
                materialsBodyRounding = mats.value("materialsBodyRounding", materialsBodyRounding);
                materialsTitleOffset = mats.value("materialsTitleOffset", materialsTitleOffset);
            } else {
                materialsBorderColor = ImVec4(45, 47, 63, 255);
                materialsTabBackgroundColor = ImVec4(26, 27, 33, 255);
                materialsTitleBackgroundColor = ImVec4(31, 32, 42, 255);
                materialsTreeBodyColor = ImVec4(28, 30, 38, 255);
            }
            if (ins.contains("assets") && ins["assets"].is_object()) {
                const json& assets = ins["assets"];
                assetsDirectoryTextColor = ImVec4(
                    assets["assetsDirectoryTextColor"][0],
                    assets["assetsDirectoryTextColor"][1],
                    assets["assetsDirectoryTextColor"][2],
                    assets["assetsDirectoryTextColor"][3]
                );
                assetsFileBackgroundColor = ImVec4(
                    assets["assetsFileBackgroundColor"][0],
                    assets["assetsFileBackgroundColor"][1],
                    assets["assetsFileBackgroundColor"][2],
                    assets["assetsFileBackgroundColor"][3]
                );
                assetsBorderColor = ImVec4(
                    assets["assetsBorderColor"][0],
                    assets["assetsBorderColor"][1],
                    assets["assetsBorderColor"][2],
                    assets["assetsBorderColor"][3]
                );
                assetsTabBackgroundColor = ImVec4(
                    assets["assetsTabBackgroundColor"][0],
                    assets["assetsTabBackgroundColor"][1],
                    assets["assetsTabBackgroundColor"][2],
                    assets["assetsTabBackgroundColor"][3]
                );
                assetsTitleBackgroundColor = ImVec4(
                    assets["assetsTitleBackgroundColor"][0],
                    assets["assetsTitleBackgroundColor"][1],
                    assets["assetsTitleBackgroundColor"][2],
                    assets["assetsTitleBackgroundColor"][3]
                );
                assetsTreeBodyColor = ImVec4(
                    assets["assetsTreeBodyColor"][0],
                    assets["assetsTreeBodyColor"][1],
                    assets["assetsTreeBodyColor"][2],
                    assets["assetsTreeBodyColor"][3]
                );
                assetsBorderThickness = assets.value("assetsBorderThickness", assetsBorderThickness);
                assetsBodyPadding = assets.value("assetsBodyPadding", assetsBodyPadding);
                assetsTitleInnerPadding = assets.value("assetsTitleInnerPadding", assetsTitleInnerPadding);
                assetsBodyRounding = assets.value("assetsBodyRounding", assetsBodyRounding);
                assetsTitleOffset = assets.value("assetsTitleOffset", assetsTitleOffset);
            } else {
                assetsDirectoryTextColor = ImVec4(180, 185, 175, 255);
                assetsFileBackgroundColor = ImVec4(40, 42, 54, 255);
                assetsBorderColor = ImVec4(45, 47, 63, 255);
                assetsTabBackgroundColor = ImVec4(26, 27, 33, 255);
                assetsTitleBackgroundColor = ImVec4(31, 32, 42, 255);
                assetsTreeBodyColor = ImVec4(28, 30, 38, 255);
            }
    
            if (ins.contains("shaders") && ins["shaders"].is_object()) {
                const json& shaders = ins["shaders"];
                
                shaderTabBackgroundColor = ImVec4(
                    shaders["shaderTabBackgroundColor"][0],
                    shaders["shaderTabBackgroundColor"][1],
                    shaders["shaderTabBackgroundColor"][2],
                    shaders["shaderTabBackgroundColor"][3]
                );
                shaderTitleBackgroundColor = ImVec4(
                    shaders["shaderTitleBackgroundColor"][0],
                    shaders["shaderTitleBackgroundColor"][1],
                    shaders["shaderTitleBackgroundColor"][2],
                    shaders["shaderTitleBackgroundColor"][3]
                );
                shaderTreeBodyColor = ImVec4(
                    shaders["shaderTreeBodyColor"][0],
                    shaders["shaderTreeBodyColor"][1],
                    shaders["shaderTreeBodyColor"][2],
                    shaders["shaderTreeBodyColor"][3]
                );
                shaderBorderColor = ImVec4(
                    shaders["shaderBorderColor"][0],
                    shaders["shaderBorderColor"][1],
                    shaders["shaderBorderColor"][2],
                    shaders["shaderBorderColor"][3]
                );

                shaderBorderThickness = shaders.value("shaderBorderThickness", shaderBorderThickness);
                shaderBodyPadding = shaders.value("shaderBodyPadding", shaderBodyPadding);
                shaderTitleInnerPadding = shaders.value("shaderTitleInnerPadding", shaderTitleInnerPadding);
                shaderBodyRounding = shaders.value("shaderBodyRounding", shaderBodyRounding);
                shaderTitleOffset = shaders.value("shaderTitleOffset", shaderTitleOffset);
            } else {
                shaderTabBackgroundColor = assetsTabBackgroundColor;
                shaderTitleBackgroundColor = assetsTitleBackgroundColor;
                shaderTreeBodyColor = assetsTreeBodyColor;
                shaderBorderColor = assetsBorderColor;
            }
        }

        // Load Console Styles
        if (s.contains("console") && s["console"].is_object()) {
            const json& cons = s["console"];
            if (cons.contains("criticalColor"))        consoleCriticalColor        = ImVec4(cons["criticalColor"][0], cons["criticalColor"][1], cons["criticalColor"][2], cons["criticalColor"][3]);
            if (cons.contains("errorColor"))           consoleErrorColor           = ImVec4(cons["errorColor"][0], cons["errorColor"][1], cons["errorColor"][2], cons["errorColor"][3]);
            if (cons.contains("warningColor"))         consoleWarningColor         = ImVec4(cons["warningColor"][0], cons["warningColor"][1], cons["warningColor"][2], cons["warningColor"][3]);
            if (cons.contains("infoColor"))            consoleInfoColor            = ImVec4(cons["infoColor"][0], cons["infoColor"][1], cons["infoColor"][2], cons["infoColor"][3]);
            if (cons.contains("searchHighlightColor")) consoleSearchHighlightColor = ImVec4(cons["searchHighlightColor"][0], cons["searchHighlightColor"][1], cons["searchHighlightColor"][2], cons["searchHighlightColor"][3]);
            if (cons.contains("windowBgColor"))        consoleWindowBgColor        = ImVec4(cons["windowBgColor"][0], cons["windowBgColor"][1], cons["windowBgColor"][2], cons["windowBgColor"][3]);
            if (cons.contains("menuBarBgColor"))       consoleMenuBarBgColor       = ImVec4(cons["menuBarBgColor"][0], cons["menuBarBgColor"][1], cons["menuBarBgColor"][2], cons["menuBarBgColor"][3]);
            if (cons.contains("textSelectedBgColor"))  consoleTextSelectedBgColor  = ImVec4(cons["textSelectedBgColor"][0], cons["textSelectedBgColor"][1], cons["textSelectedBgColor"][2], cons["textSelectedBgColor"][3]);
        } 
        else {
            // fallback to the default theme colors 
            consoleCriticalColor        = ImVec4(1.00000f, 0.33333f, 0.33333f, 1.00f);
            consoleErrorColor           = ImVec4(1.00000f, 0.47451f, 0.77647f, 1.00f);
            consoleWarningColor         = ImVec4(1.00000f, 0.72157f, 0.42353f, 1.00f);
            consoleInfoColor            = ImVec4(0.31373f, 0.98039f, 0.48235f, 1.00f);
            consoleWindowBgColor        = ImVec4(0.15686f, 0.16471f, 0.21176f, 1.00f);
            consoleMenuBarBgColor       = ImVec4(0.12157f, 0.12549f, 0.16471f, 1.00f);
            consoleTextSelectedBgColor  = ImVec4(0.26667f, 0.27843f, 0.35294f, 0.80f);
            consoleSearchHighlightColor = ImVec4(0.74118f, 0.57647f, 0.97647f, 0.50f);
        }
    }
};
