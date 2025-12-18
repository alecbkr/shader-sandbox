#include "UIContext.hpp"

#include "core/EditorEngine.hpp"

void styleImGui(ImGuiIO& io);

UIContext::UIContext(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    styleImGui(io);
}

void UIContext::preRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIContext::postRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIContext::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (EditorUI* editor: EditorEngine::editors) editor->destroy();
}

void UIContext::renderEditorWindow(float width, float height) {
    ImGui::SetNextWindowSize(ImVec2( width, height), ImGuiCond_Once);

    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    EditorEngine::spawnEditor(1024);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::BeginTabBar("EditorTabs")) {
            for (int i = 0; i < EditorEngine::editors.size(); i++) {
                std::string tabTitle = "File " + std::to_string(i + 1);
                bool openTab = true;
                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {

                    EditorEngine::editors[i]->render();

                    ImGui::EndTabItem();
                }

                if (!openTab) {
                    EditorEngine::editors[i]->destroy();
                    EditorEngine::editors.erase(EditorEngine::editors.begin() + i);
                }
            }

            ImGui::EndTabBar();
        }

    }
    ImGui::End();
}

void UIContext::render(InspectorUI& inspectorUI) {
    inspectorUI.render();
}

static int g_SelectedStyleColor = ImGuiCol_Text;

void UIContext::render(MenuUI& menuUI) {
    menuUI.render();
    
    ImGuiStyle& style = ImGui::GetStyle();

    if (ImGui::Begin("Styling", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::SliderFloat("Alpha", &style.Alpha, 0.1f, 1.0f, "%0.2f");
        ImGui::SliderFloat("Disabled Alpha", &style.DisabledAlpha, 0.1f, 1.0f, "%0.2f");
        ImGui::SliderFloat2("Window Padding", &style.WindowPadding.x, 1.0f, 20.0f, "%0.1f");
        ImGui::SliderFloat("Window Rounding", &style.WindowRounding, 0.0f, 10.0f, "%0.1f");
        ImGui::SliderFloat("Window Border Size", &style.WindowBorderSize, 0.0f, 2.0f, "%0.2f");
        ImGui::SliderFloat("Window Border Hover Padding", &style.WindowBorderHoverPadding, 0.1f, 8.0f, "%0.1f");
        ImGui::SliderFloat2("Window Minimum Size", &style.WindowMinSize.x, 120.0f, 400.0f, "%0.0f");
        ImGui::SliderFloat2("Window Title Align", &style.WindowTitleAlign.x, 0.0f, 1.0f, "%0.2f");
        {
            int pos = 0;
            if (style.WindowMenuButtonPosition == ImGuiDir_None)  pos = 0;
            else if (style.WindowMenuButtonPosition == ImGuiDir_Left)  pos = 1;
            else if (style.WindowMenuButtonPosition == ImGuiDir_Right) pos = 2;
            const char* items[] = { "None", "Left", "Right" };
            if (ImGui::Combo("Window Menu Button Position", &pos, items, IM_ARRAYSIZE(items))) {
                if      (pos == 0) style.WindowMenuButtonPosition = ImGuiDir_None;
                else if (pos == 1) style.WindowMenuButtonPosition = ImGuiDir_Left;
                else if (pos == 2) style.WindowMenuButtonPosition = ImGuiDir_Right;
            }
        }
        ImGui::SliderFloat("Child Rounding", &style.ChildRounding, 0.0f, 10.0f, "%0.1f");
        ImGui::SliderFloat("Child Border Size", &style.ChildBorderSize, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat("Popup Rounding", &style.PopupRounding, 0.0f, 10.0f, "%0.1f");
        ImGui::SliderFloat("Popup Border Size", &style.PopupBorderSize, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat2("Frame Padding", &style.FramePadding.x, 2.0f, 16.0f, "%0.1f");
        ImGui::SliderFloat("Frame Rounding", &style.FrameRounding, 0.0f, 10.0f, "%0.1f");
        ImGui::SliderFloat("Frame Border Size", &style.FrameBorderSize, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat2("Item Spacing", &style.ItemSpacing.x, 2.0f, 20.0f, "%0.1f");
        ImGui::SliderFloat2("Item Inner Spacing", &style.ItemInnerSpacing.x, 0.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat2("Cell Padding", &style.CellPadding.x, 1.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat2("Touch Extra Padding", &style.TouchExtraPadding.x, 0.0f, 10.0f, "%0.1f");
        ImGui::SliderFloat("Indent Spacing", &style.IndentSpacing, 14.0f, 40.0f, "%0.0f");
        ImGui::SliderFloat("Columns Minimum Spacing", &style.ColumnsMinSpacing, 2.0f, 30.0f, "%0.0f");
        ImGui::SliderFloat("Scrollbar Size", &style.ScrollbarSize, 5.0f, 25.0f, "%0.0f");
        ImGui::SliderFloat("Scrollbar Rounding", &style.ScrollbarRounding, 0.0f, 15.0f, "%0.1f");
        ImGui::SliderFloat("Scrollbar Padding", &style.ScrollbarPadding, 0.0f, 5.0f, "%0.2f");
        ImGui::SliderFloat("Grab Min Size", &style.GrabMinSize, 4.0f, 30.0f, "%0.1f");
        ImGui::SliderFloat("Grab Rounding", &style.GrabRounding, 0.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat("Log Slider Deadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat("Image Border Size", &style.ImageBorderSize, 0.0f, 4.0f, "%0.1f");
        ImGui::SliderFloat("Tab Rounding", &style.TabRounding, 0.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat("Tab Border Size", &style.TabBorderSize, 0.0f, 2.0f, "%0.2f");
        ImGui::SliderFloat("Tab Min Width Base", &style.TabMinWidthBase, 0.0f, 250.0f, "%0.0f");
        ImGui::SliderFloat("Tab Min Width Shrink", &style.TabMinWidthShrink, 0.0f, 250.0f, "%0.0f");
        ImGui::SliderFloat("Tab CloseBtn Min W (Selected)", &style.TabCloseButtonMinWidthSelected, -1.0f, 120.0f, "%0.1f");
        ImGui::SliderFloat("Tab CloseBtn Min W (Unselected)", &style.TabCloseButtonMinWidthUnselected, -1.0f, 120.0f, "%0.1f");
        ImGui::SliderFloat("TabBar Border Size", &style.TabBarBorderSize, 0.0f, 3.0f, "%0.2f");
        ImGui::SliderFloat("TabBar Overline Size", &style.TabBarOverlineSize, 0.0f, 4.0f, "%0.2f");
        ImGui::SliderFloat("Table Angled Headers Angle", &style.TableAngledHeadersAngle, -50.0f, 50.0f, "%0.1f");
        ImGui::SliderFloat2("Table Angled Headers Text Align", &style.TableAngledHeadersTextAlign.x, 0.0f, 1.0f, "%0.2f");
        {
            int mode = 0;
            if (style.TreeLinesFlags == ImGuiTreeNodeFlags_DrawLinesFull) mode = 1;
            else if (style.TreeLinesFlags == ImGuiTreeNodeFlags_DrawLinesToNodes) mode = 2;
            else mode = 0;
            const char* items[] = { "None", "Full", "To Nodes" };
            if (ImGui::Combo("Tree Lines Mode", &mode, items, IM_ARRAYSIZE(items))) {
                if      (mode == 0) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesNone;
                else if (mode == 1) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesFull;
                else if (mode == 2) style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesToNodes;
            }
            if (style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesNone &&
                style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesFull &&
                style.TreeLinesFlags != ImGuiTreeNodeFlags_DrawLinesToNodes) {
                style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesNone;
            }
        }
        ImGui::SliderFloat("Tree Lines Size", &style.TreeLinesSize, 0.0f, 5.0f, "%0.2f");
        ImGui::SliderFloat("Tree Lines Rounding", &style.TreeLinesRounding, 0.0f, 8.0f, "%0.1f");
        ImGui::SliderFloat("DragDrop Target Rounding", &style.DragDropTargetRounding, 0.0f, 12.0f, "%0.1f");
        ImGui::SliderFloat("DragDrop Target Border Size", &style.DragDropTargetBorderSize, 0.0f, 3.0f, "%0.2f");
        ImGui::SliderFloat("DragDrop Target Padding", &style.DragDropTargetPadding, 0.0f, 12.0f, "%0.1f");
        {
            int dir = (style.ColorButtonPosition == ImGuiDir_Left) ? 0 : 1;
            const char* items[] = { "Left", "Right" };
            if (ImGui::Combo("Color Button Position", &dir, items, IM_ARRAYSIZE(items))) {
                style.ColorButtonPosition = (dir == 0) ? ImGuiDir_Left : ImGuiDir_Right;
            }
        }
        ImGui::SliderFloat2("Button Text Align", &style.ButtonTextAlign.x, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat2("Selectable Text Align", &style.SelectableTextAlign.x, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat("Separator Text Border Size", &style.SeparatorTextBorderSize, 0.0f, 3.0f, "%0.2f");
        ImGui::SliderFloat2("Separator Text Align", &style.SeparatorTextAlign.x, 0.0f, 1.0f, "%0.2f");
        ImGui::SliderFloat2("Separator Text Padding", &style.SeparatorTextPadding.x, 0.0f, 24.0f, "%0.1f");
        ImGui::SliderFloat2("Display Window Padding", &style.DisplayWindowPadding.x, 0.0f, 40.0f, "%0.0f");
        ImGui::SliderFloat2("Display Safe Area Padding", &style.DisplaySafeAreaPadding.x, 0.0f, 40.0f, "%0.0f");
        ImGui::SliderFloat("Mouse Cursor Scale", &style.MouseCursorScale, 0.5f, 3.0f, "%0.2f");
        ImGui::Checkbox("AntiAliased Lines", &style.AntiAliasedLines);
        ImGui::Checkbox("AntiAliased Lines Use Tex", &style.AntiAliasedLinesUseTex);
        ImGui::Checkbox("AntiAliased Fill", &style.AntiAliasedFill);
        ImGui::SliderFloat("Curve Tessellation Tol", &style.CurveTessellationTol, 0.5f, 8.0f, "%0.2f");
        ImGui::SliderFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError, 0.5f, 8.0f, "%0.2f");
        static const char* kColorNames[ImGuiCol_COUNT] = {
            "Text", "TextDisabled", "WindowBg", "ChildBg", "PopupBg", "Border", "BorderShadow", "FrameBg", "FrameBgHovered", "FrameBgActive", "TitleBg", "TitleBgActive",
            "TitleBgCollapsed", "MenuBarBg", "ScrollbarBg", "ScrollbarGrab", "ScrollbarGrabHovered", "ScrollbarGrabActive", "CheckMark", "SliderGrab", "SliderGrabActive",
            "Button", "ButtonHovered", "ButtonActive", "Header", "HeaderHovered", "HeaderActive", "Separator", "SeparatorHovered", "SeparatorActive", "ResizeGrip",
            "ResizeGripHovered", "ResizeGripActive", "InputTextCursor", "TabHovered", "Tab", "TabSelected", "TabSelectedOverline", "TabDimmed", "TabDimmedSelected",
            "TabDimmedSelectedOverline", "PlotLines", "PlotLinesHovered", "PlotHistogram", "PlotHistogramHovered", "TableHeaderBg", "TableBorderStrong", "TableBorderLight",
            "TableRowBg", "TableRowBgAlt", "TextLink", "TextSelectedBg", "TreeLines", "DragDropTarget", "DragDropTargetBg", "UnsavedMarker", "NavCursor",
            "NavWindowingHighlight", "NavWindowingDimBg", "ModalWindowDimBg"
        };
        ImGui::Combo("Style Color", &g_SelectedStyleColor, kColorNames, ImGuiCol_COUNT);
        ImGui::SameLine();
        ImGui::ColorButton("##style_color_preview", style.Colors[g_SelectedStyleColor], ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(20, 20));
        ImGui::ColorPicker4("Picker", &style.Colors[g_SelectedStyleColor].x);
    }
    ImGui::End();
}

void styleImGui(ImGuiIO& io) {
    ImGuiStyle& style = ImGui::GetStyle();

    // Dark Colors are a better base
    ImGui::StyleColorsDark();
    
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(12, 12);

    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.33f, 0.33f, 0.35f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);

    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
}