#pragma once

#include <algorithm>
#include <imgui/imgui.h>
#include "application/SettingsStyles.hpp"

namespace DefaultTheme {

    inline void applyDefaultTheme(SettingsStyles& styles) {
        // ---------- Core style values ----------
        styles.alpha = 1.0f;
        styles.disabledAlpha = 0.6000000238418579f;

        styles.windowPadding = ImVec2(8.0f, 6.0f);
        styles.windowRounding = 0.0f;
        styles.windowBorderSize = 1.0f;
        styles.windowMinSize = ImVec2(120.0f, 120.0f);
        styles.windowTitleAlign = ImVec2(0.0f, 0.5f);
        styles.windowMenuButtonPosition = ImGuiDir_Left;
        styles.windowBorderHoverPadding = 4.0f;

        styles.childRounding = 4.0f;
        styles.childBorderSize = 1.0f;

        styles.popupRounding = 0.0f;
        styles.popupBorderSize = 1.0f;

        styles.framePadding = ImVec2(4.0f, 11.0f);
        styles.frameRounding = 0.0f;
        styles.frameBorderSize = 0.0f;

        styles.itemSpacing = ImVec2(8.0f, 4.0f);
        styles.itemInnerSpacing = ImVec2(4.0f, 4.0f);
        styles.cellPadding = ImVec2(4.0f, 2.0f);
        styles.touchExtraPadding = ImVec2(0.0f, 0.0f);
        styles.indentSpacing = 21.0f;
        styles.columnsMinSpacing = 6.0f;

        styles.scrollbarSize = 14.0f;
        styles.scrollbarRounding = 9.0f;
        styles.scrollbarPadding = 2.0f;

        styles.grabMinSize = 10.0f;
        styles.grabRounding = 0.0f;

        styles.logSliderDeadzone = 4.0f;
        styles.imageBorderSize = 0.0f;

        styles.tabRounding = 4.0f;
        styles.tabBorderSize = 0.0f;
        styles.tabBarBorderSize = 1.0f;
        styles.tabBarOverlineSize = 2.0f;
        styles.tabCloseButtonMinWidthSelected = 0.0f;
        styles.tabCloseButtonMinWidthUnselected = 0.0f;
        styles.tabMinWidthBase = 20.0f;
        styles.tabMinWidthShrink = 0.0f;

        styles.tableAngledHeadersAngle = 35.0f;
        styles.tableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);

        styles.separatorTextBorderSize = 0.0f;
        styles.separatorTextAlign = ImVec2(0.0f, 0.5f);
        styles.separatorTextPadding = ImVec2(20.0f, 3.0f);

        styles.displayWindowPadding = ImVec2(19.0f, 19.0f);
        styles.displaySafeAreaPadding = ImVec2(3.0f, 3.0f);

        styles.mouseCursorScale = 1.0f;

        styles.antiAliasedLines = true;
        styles.antiAliasedLinesUseTex = true;
        styles.antiAliasedFill = true;

        styles.curveTessellationTol = 1.25f;
        styles.circleTessellationMaxError = 0.30000001192092896f;

        styles.colorButtonPosition = ImGuiDir_Right;
        styles.buttonTextAlign = ImVec2(0.5f, 0.5f);
        styles.selectableTextAlign = ImVec2(0.0f, 0.0f);

        styles.dragDropTargetPadding = 0.0f;
        styles.dragDropTargetRounding = 0.0f;
        styles.dragDropTargetBorderSize = 0.0f;

        styles.treeLinesFlags = 262144;
        styles.treeLinesSize = 0.0f;
        styles.treeLinesRounding = 0.0f;

        // ---------- ImGui colors ----------
        const ImVec4 defaultColors[] = {
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(0.3843137323856354f, 0.4470588266849518f, 0.6431372761726379f, 1.0f),
            ImVec4(0.1568627506494522f, 0.16470588743686676f, 0.21176470816135406f, 1.0f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(0.16862745583057404f, 0.1764705926179886f, 0.22745098173618317f, 1.0f),
            ImVec4(0.05886966362595558f, 0.0627385824918747f, 0.08627451211214066f, 0.6000000238418579f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(0.22745098173618317f, 0.23529411852359772f, 0.30588236451148987f, 1.0f),
            ImVec4(0.3019607961177826f, 0.3137255012989044f, 0.43921568989753723f, 1.0f),
            ImVec4(0.2666666805744171f, 0.27843138575553894f, 0.3529411852359772f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.12143021821975708f, 0.12550322711467743f, 0.16470588743686676f, 0.7490196228027344f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.2666666805744171f, 0.27843138575553894f, 0.3529411852359772f, 1.0f),
            ImVec4(0.33725491166114807f, 0.3529411852359772f, 0.4588235318660736f, 1.0f),
            ImVec4(0.41960784792900085f, 0.43921568989753723f, 0.5568627715110779f, 1.0f),
            ImVec4(0.3137255012989044f, 0.9803921580314636f, 0.48235294222831726f, 1.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.22745098173618317f, 0.23529411852359772f, 0.30588236451148987f, 1.0f),
            ImVec4(0.33725491166114807f, 0.3529411852359772f, 0.4588235318660736f, 1.0f),
            ImVec4(0.41960784792900085f, 0.43921568989753723f, 0.5568627715110779f, 1.0f),
            ImVec4(0.21960784494876862f, 0.22745098173618317f, 0.3019607961177826f, 1.0f),
            ImVec4(0.3019607961177826f, 0.3137255012989044f, 0.43921568989753723f, 1.0f),
            ImVec4(0.3843137323856354f, 0.4470588266849518f, 0.6431372761726379f, 1.0f),
            ImVec4(0.05886966362595558f, 0.0627385824918747f, 0.08627451211214066f, 0.6000000238418579f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.26143789291381836f, 0.5828526616096497f, 0.9803921580314636f, 0.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(0.3019607961177826f, 0.3137255012989044f, 0.43921568989753723f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.21960784494876862f, 0.22745098173618317f, 0.3019607961177826f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.16862745583057404f, 0.1764705926179886f, 0.22745098173618317f, 1.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(0.6100000143051147f, 0.6100000143051147f, 0.6100000143051147f, 1.0f),
            ImVec4(1.0f, 0.7215686440467834f, 0.42352941632270813f, 1.0f),
            ImVec4(0.8999999761581421f, 0.699999988079071f, 0.0f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f),
            ImVec4(0.05886966362595558f, 0.0627385824918747f, 0.08627451211214066f, 0.800000011920929f),
            ImVec4(0.05886966362595558f, 0.0627385824918747f, 0.08627451211214066f, 0.3529411852359772f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f),
            ImVec4(0.545098066329956f, 0.9137254953384399f, 0.9921568632125854f, 1.0f),
            ImVec4(0.38336023688316345f, 0.44448423385620117f, 0.6431372761726379f, 0.800000011920929f),
            ImVec4(0.2666666805744171f, 0.27843138575553894f, 0.3529411852359772f, 1.0f),
            ImVec4(1.0f, 0.7215686440467834f, 0.42352941632270813f, 1.0f),
            ImVec4(1.0f, 0.7219376564025879f, 0.42352938652038574f, 0.20000000298023224f),
            ImVec4(1.0f, 0.3333333432674408f, 0.3333333432674408f, 1.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.09803921729326248f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.20000000298023224f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.3499999940395355f),
        };

        for (int i = 0; i < ImGuiCol_COUNT && i < static_cast<int>(sizeof(defaultColors) / sizeof(defaultColors[0])); ++i) {
            styles.colors[i] = defaultColors[i];
        }

        // ---------- Text editor palette ----------
        const ImVec4 editorPalette[] = {
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(1.0f, 0.4745098054409027f, 0.7764706015586853f, 1.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
            ImVec4(0.9450980424880981f, 0.9803921580314636f, 0.5490196347236633f, 1.0f),
            ImVec4(0.9450980424880981f, 0.9803921580314636f, 0.5490196347236633f, 1.0f),
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(1.0f, 0.7215686440467834f, 0.42352941632270813f, 1.0f),
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(0.3137255012989044f, 0.9803921580314636f, 0.48235294222831726f, 1.0f),
            ImVec4(0.545098066329956f, 0.9137254953384399f, 0.9921568632125854f, 1.0f),
            ImVec4(0.3137255012989044f, 0.9803921580314636f, 0.48235294222831726f, 1.0f),
            ImVec4(0.3843137323856354f, 0.4470588266849518f, 0.6431372761726379f, 1.0f),
            ImVec4(0.3843137323856354f, 0.4470588266849518f, 0.6431372761726379f, 1.0f),
            ImVec4(0.1568627506494522f, 0.16470588743686676f, 0.21176470816135406f, 1.0f),
            ImVec4(0.9725490212440491f, 0.9725490212440491f, 0.9490196108818054f, 1.0f),
            ImVec4(0.2666666805744171f, 0.27843138575553894f, 0.3529411852359772f, 1.0f),
            ImVec4(1.0f, 0.3333333432674408f, 0.3333333432674408f, 1.0f),
            ImVec4(1.0f, 0.3333333432674408f, 0.3333333432674408f, 1.0f),
            ImVec4(0.3843137323856354f, 0.4470588266849518f, 0.6431372761726379f, 1.0f),
            ImVec4(0.2666666805744171f, 0.27843138575553894f, 0.3529411852359772f, 1.0f),
            ImVec4(0.20392157137393951f, 0.21568627655506134f, 0.27450981736183167f, 1.0f),
            ImVec4(0.7411764860153198f, 0.5764706134796143f, 0.9764705896377563f, 1.0f),
        };

        for (int i = 0; i < static_cast<int>(sizeof(editorPalette) / sizeof(editorPalette[0])); ++i) {
            styles.editorPalette[i] = editorPalette[i];
        }

        styles.paletteVersion++;

        // ---------- Custom inspector values ----------
        styles.assetsBodyPadding = 12.0f;
        styles.assetsBodyRounding = 6.0f;
        styles.assetsBorderThickness = 1.0f;
        styles.assetsTitleInnerPadding = 1.0f;
        styles.assetsTitleOffset = 6.0f;

        styles.assetsBorderColor = ImVec4(0.1764705926179886f, 0.18431372940540314f, 0.24705882370471954f, 1.0f);
        styles.assetsDirectoryTextColor = ImVec4(0.7058823704719543f, 0.7254902124404907f, 0.686274528503418f, 1.0f);
        styles.assetsFileBackgroundColor = ImVec4(0.1568627506494522f, 0.16470588743686676f, 0.21176470816135406f, 1.0f);
        styles.assetsTabBackgroundColor = ImVec4(0.10196078568696976f, 0.10588235408067703f, 0.12941177189350128f, 1.0f);
        styles.assetsTitleBackgroundColor = ImVec4(0.12156862765550613f, 0.125490203499794f, 0.16470588743686676f, 1.0f);
        styles.assetsTreeBodyColor = ImVec4(0.10980392247438431f, 0.11764705926179886f, 0.14901961386203766f, 1.0f);
        styles.inspectorTreeHoveredColor = ImVec4(0.16862745583057404f, 0.1764705926179886f, 0.22745098173618317f, 1.0f);

        // ---------- Custom console values ----------
        // Foreground & Log Levels
        styles.consoleCriticalColor        = ImVec4(1.00000f, 0.33333f, 0.33333f, 1.00f); // Red (#ff5555)
        styles.consoleErrorColor           = ImVec4(1.00000f, 0.47451f, 0.77647f, 1.00f); // Pink (#ff79c6)
        styles.consoleWarningColor         = ImVec4(1.00000f, 0.72157f, 0.42353f, 1.00f); // Orange (#ffb86c)
        styles.consoleInfoColor            = ImVec4(0.31373f, 0.98039f, 0.48235f, 1.00f); // Green (#50fa7b)
        
        // Window & UI Elements
        styles.consoleWindowBgColor        = ImVec4(0.15686f, 0.16471f, 0.21176f, 1.00f); // Background (#282a36)
        styles.consoleMenuBarBgColor       = ImVec4(0.13922f, 0.14510f, 0.18824f, 1.00f); // Darker Background (#1f202a)
        styles.consoleTextSelectedBgColor  = ImVec4(0.26667f, 0.27843f, 0.35294f, 0.80f); // Selection (#44475a)
        styles.consoleSearchHighlightColor = ImVec4(0.74118f, 0.57647f, 0.97647f, 0.50f); // Purple Accent (#bd93f9)


        styles.shaderBodyPadding = 12.0f;
        styles.shaderBodyRounding = 6.0f;
        styles.shaderBorderThickness = 1.0f;
        styles.shaderTitleInnerPadding = 1.0f;
        styles.shaderTitleOffset = 6.0f;

        styles.shaderBorderColor = styles.assetsBorderColor;
        styles.shaderTabBackgroundColor = styles.assetsTabBackgroundColor;
        styles.shaderTitleBackgroundColor = styles.assetsTitleBackgroundColor;
        styles.shaderTreeBodyColor = styles.assetsTreeBodyColor;
        // Push everything into ImGui
        styles.applyToImGui(ImGui::GetStyle());
    }

} // namespace DefaultTheme
