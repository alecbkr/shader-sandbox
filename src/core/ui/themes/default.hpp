#pragma once

#include <imgui/imgui.h>
#include "application/SettingsStyles.hpp"

struct ThemeColor {
    float r, g, b, a;
    constexpr ImVec4 v() const { return ImVec4(r, g, b, a); }
};

namespace DefaultTheme {
    // Core hierarchy:
    // APP_BG        = darkest shell
    // PANEL_BG      = main work areas
    // SURFACE_1     = slightly lifted surfaces
    // SURFACE_2     = hovered / selected neutrals
    // SURFACE_3     = strongest neutral lift
    //
    // These are intentionally close together to preserve the dracula-style depth structure,
    // just shifted into your blue/purple family.

    constexpr ThemeColor APP_BG         { 0.045f, 0.055f, 0.095f, 1.0f };
    constexpr ThemeColor PANEL_BG       { 0.095f, 0.105f, 0.165f, 1.0f };
    constexpr ThemeColor SURFACE_1      { 0.125f, 0.135f, 0.205f, 1.0f };
    constexpr ThemeColor SURFACE_2      { 0.165f, 0.180f, 0.270f, 1.0f };
    constexpr ThemeColor SURFACE_3      { 0.205f, 0.225f, 0.335f, 1.0f };

    constexpr ThemeColor TITLE_BG       { 0.070f, 0.080f, 0.130f, 1.0f };
    constexpr ThemeColor MENU_BG        { 0.060f, 0.070f, 0.115f, 1.0f };
    constexpr ThemeColor POPUP_BG       { 0.100f, 0.112f, 0.175f, 1.0f };

    constexpr ThemeColor BORDER         { 0.210f, 0.240f, 0.380f, 1.0f };
    constexpr ThemeColor BORDER_SOFT    { 0.140f, 0.160f, 0.250f, 1.0f };
    constexpr ThemeColor SHADOW         { 0.000f, 0.000f, 0.000f, 0.000f };

    constexpr ThemeColor TEXT_PRIMARY   { 0.930f, 0.940f, 0.985f, 1.0f };
    constexpr ThemeColor TEXT_SECONDARY { 0.640f, 0.690f, 0.840f, 1.0f };

    constexpr ThemeColor ACCENT_BLUE    { 0.300f, 0.520f, 0.980f, 1.0f };
    constexpr ThemeColor ACCENT_CYAN    { 0.360f, 0.820f, 0.980f, 1.0f };
    constexpr ThemeColor ACCENT_PURPLE  { 0.690f, 0.500f, 0.980f, 1.0f };
    constexpr ThemeColor ACCENT_PINK    { 0.980f, 0.430f, 0.760f, 1.0f };

    constexpr ThemeColor ACCENT_BLUE_SOFT   { 0.300f, 0.520f, 0.980f, 0.60f };
    constexpr ThemeColor ACCENT_CYAN_SOFT   { 0.360f, 0.820f, 0.980f, 0.20f };
    constexpr ThemeColor ACCENT_PURPLE_SOFT { 0.690f, 0.500f, 0.980f, 0.35f };

    constexpr ThemeColor WHITE_06       { 1.000f, 1.000f, 1.000f, 0.06f };
    constexpr ThemeColor WHITE_10       { 1.000f, 1.000f, 1.000f, 0.10f };
    constexpr ThemeColor BLACK_20       { 0.000f, 0.000f, 0.000f, 0.20f };
    constexpr ThemeColor BLACK_35       { 0.000f, 0.000f, 0.000f, 0.35f };
    constexpr ThemeColor BLACK_60       { 0.000f, 0.000f, 0.000f, 0.60f };

    constexpr ThemeColor ZERO           { 0.000f, 0.000f, 0.000f, 0.000f };

    constexpr ThemeColor ERROR_RED      { 1.000f, 0.380f, 0.380f, 1.0f };
    constexpr ThemeColor FUNC_GREEN     { 0.460f, 0.940f, 0.600f, 1.0f };

    // Assets inspector custom colors
    constexpr ThemeColor ASSET_TAB_BG    { 0.080f, 0.090f, 0.145f, 1.0f };
    constexpr ThemeColor ASSET_TITLE_BG  { 0.105f, 0.118f, 0.185f, 1.0f };
    constexpr ThemeColor ASSET_TREE_BG   { 0.075f, 0.085f, 0.135f, 1.0f };
    constexpr ThemeColor ASSET_FILE_BG   { 0.145f, 0.155f, 0.235f, 1.0f };
    constexpr ThemeColor ASSET_DIR_TEXT  { 0.720f, 0.790f, 0.980f, 1.0f };
}

inline void applyDefaultTheme(SettingsStyles& styles) {
    using namespace DefaultTheme;

    auto& c = styles.colors;

    c[ImGuiCol_Text]                 = TEXT_PRIMARY.v();
    c[ImGuiCol_TextDisabled]         = TEXT_SECONDARY.v();
    c[ImGuiCol_WindowBg]             = APP_BG.v();
    c[ImGuiCol_ChildBg]              = ZERO.v();
    c[ImGuiCol_PopupBg]              = POPUP_BG.v();
    c[ImGuiCol_Border]               = BORDER.v();
    c[ImGuiCol_BorderShadow]         = SHADOW.v();

    c[ImGuiCol_FrameBg]              = SURFACE_1.v();
    c[ImGuiCol_FrameBgHovered]       = SURFACE_2.v();
    c[ImGuiCol_FrameBgActive]        = SURFACE_3.v();

    c[ImGuiCol_TitleBg]              = TITLE_BG.v();
    c[ImGuiCol_TitleBgActive]        = TITLE_BG.v();
    c[ImGuiCol_TitleBgCollapsed]     = ImVec4(TITLE_BG.r, TITLE_BG.g, TITLE_BG.b, 0.75f);
    c[ImGuiCol_MenuBarBg]            = MENU_BG.v();

    c[ImGuiCol_ScrollbarBg]          = TITLE_BG.v();
    c[ImGuiCol_ScrollbarGrab]        = SURFACE_1.v();
    c[ImGuiCol_ScrollbarGrabHovered] = SURFACE_2.v();
    c[ImGuiCol_ScrollbarGrabActive]  = SURFACE_3.v();

    c[ImGuiCol_CheckMark]            = ACCENT_CYAN.v();
    c[ImGuiCol_SliderGrab]           = ACCENT_PURPLE.v();
    c[ImGuiCol_SliderGrabActive]     = ACCENT_PINK.v();

    c[ImGuiCol_Button]               = SURFACE_1.v();
    c[ImGuiCol_ButtonHovered]        = SURFACE_2.v();
    c[ImGuiCol_ButtonActive]         = ACCENT_BLUE_SOFT.v();

    c[ImGuiCol_Header]               = SURFACE_1.v();
    c[ImGuiCol_HeaderHovered]        = SURFACE_2.v();
    c[ImGuiCol_HeaderActive]         = ACCENT_PURPLE_SOFT.v();

    c[ImGuiCol_Separator]            = BORDER_SOFT.v();
    c[ImGuiCol_SeparatorHovered]     = ACCENT_PURPLE.v();
    c[ImGuiCol_SeparatorActive]      = ACCENT_PINK.v();

    c[ImGuiCol_ResizeGrip]           = ZERO.v();
    c[ImGuiCol_ResizeGripHovered]    = ACCENT_PURPLE.v();
    c[ImGuiCol_ResizeGripActive]     = ACCENT_PINK.v();

    c[ImGuiCol_TabHovered]           = SURFACE_2.v();
    c[ImGuiCol_Tab]                  = SURFACE_1.v();

#ifdef ImGuiCol_TabActive
    c[ImGuiCol_TabActive]            = PANEL_BG.v();
#endif
#ifdef ImGuiCol_TabSelected
    c[ImGuiCol_TabSelected]          = PANEL_BG.v();
#endif
#ifdef ImGuiCol_TabSelectedOverline
    c[ImGuiCol_TabSelectedOverline]  = ACCENT_PURPLE.v();
#endif
#ifdef ImGuiCol_TabDimmed
    c[ImGuiCol_TabDimmed]            = SURFACE_1.v();
#endif
#ifdef ImGuiCol_TabDimmedSelected
    c[ImGuiCol_TabDimmedSelected]    = PANEL_BG.v();
#endif
#ifdef ImGuiCol_TabDimmedSelectedOverline
    c[ImGuiCol_TabDimmedSelectedOverline] = BORDER_SOFT.v();
#endif

    c[ImGuiCol_PlotLines]            = ACCENT_CYAN.v();
    c[ImGuiCol_PlotLinesHovered]     = ACCENT_BLUE.v();
    c[ImGuiCol_PlotHistogram]        = ACCENT_PINK.v();
    c[ImGuiCol_PlotHistogramHovered] = ACCENT_PURPLE.v();

    c[ImGuiCol_TableHeaderBg]        = TITLE_BG.v();
    c[ImGuiCol_TableBorderStrong]    = BORDER.v();
    c[ImGuiCol_TableBorderLight]     = BORDER_SOFT.v();
    c[ImGuiCol_TableRowBg]           = ZERO.v();
    c[ImGuiCol_TableRowBgAlt]        = WHITE_06.v();

#ifdef ImGuiCol_TextLink
    c[ImGuiCol_TextLink]             = ACCENT_BLUE.v();
#endif

    c[ImGuiCol_TextSelectedBg]       = ACCENT_PURPLE_SOFT.v();
    c[ImGuiCol_DragDropTarget]       = ACCENT_CYAN.v();

#ifdef ImGuiCol_NavCursor
    c[ImGuiCol_NavCursor]            = ACCENT_CYAN_SOFT.v();
#else
    c[ImGuiCol_NavHighlight]         = ACCENT_CYAN_SOFT.v();
#endif

    c[ImGuiCol_NavWindowingHighlight]= TEXT_PRIMARY.v();
    c[ImGuiCol_NavWindowingDimBg]    = BLACK_20.v();
    c[ImGuiCol_ModalWindowDimBg]     = BLACK_35.v();

    styles.applyToImGui(ImGui::GetStyle());

    // Editor palette
    styles.editorPalette[0]  = TEXT_PRIMARY.v();    // Default
    styles.editorPalette[1]  = ACCENT_PINK.v();     // Keyword
    styles.editorPalette[2]  = ACCENT_PURPLE.v();   // Number
    styles.editorPalette[3]  = ACCENT_CYAN.v();     // String
    styles.editorPalette[4]  = ACCENT_CYAN.v();     // CharLiteral
    styles.editorPalette[5]  = TEXT_PRIMARY.v();    // Punctuation
    styles.editorPalette[6]  = ACCENT_BLUE.v();     // Preprocessor
    styles.editorPalette[7]  = TEXT_PRIMARY.v();    // Identifier
    styles.editorPalette[8]  = FUNC_GREEN.v();      // Function
    styles.editorPalette[9]  = ACCENT_CYAN.v();     // KnownIdentifier
    styles.editorPalette[10] = FUNC_GREEN.v();      // PreprocIdentifier
    styles.editorPalette[11] = ImVec4(0.430f, 0.520f, 0.760f, 1.0f); // Comment
    styles.editorPalette[12] = ImVec4(0.430f, 0.520f, 0.760f, 1.0f); // MultiLineComment
    styles.editorPalette[13] = PANEL_BG.v();        // Background
    styles.editorPalette[14] = TEXT_PRIMARY.v();    // Cursor
    styles.editorPalette[15] = ImVec4(0.220f, 0.240f, 0.360f, 1.0f); // Selection
    styles.editorPalette[16] = ERROR_RED.v();       // ErrorMarker
    styles.editorPalette[17] = ERROR_RED.v();       // Breakpoint
    styles.editorPalette[18] = ImVec4(0.340f, 0.430f, 0.720f, 1.0f); // LineNumber
    styles.editorPalette[19] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);      // CurrentLineFill
    styles.editorPalette[20] = ImVec4(1.0f, 1.0f, 1.0f, 0.015f);     // CurrentLineFillInactive
    styles.editorPalette[21] = ACCENT_PURPLE.v();   // CurrentLineEdge

    styles.paletteVersion++;

    // Assets inspector custom colors
    styles.assetsBorderColor          = BORDER.v();
    styles.assetsDirectoryTextColor   = ASSET_DIR_TEXT.v();
    styles.assetsFileBackgroundColor  = ASSET_FILE_BG.v();
    styles.assetsTabBackgroundColor   = ASSET_TAB_BG.v();
    styles.assetsTitleBackgroundColor = ASSET_TITLE_BG.v();
    styles.assetsTreeBodyColor        = ASSET_TREE_BG.v();
}