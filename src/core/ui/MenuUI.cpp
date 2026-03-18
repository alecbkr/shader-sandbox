#include "core/ui/MenuUI.hpp"

#include <glad/glad.h>
#include <stb_image.h>

#include "application/AppContext.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/Keybinds.hpp"
#include "core/logging/Logger.hpp"
#include "platform/Platform.hpp"

namespace
{
    // ---- Window button icon drawers (all centered + symmetric) ----

    static void DrawMinimizeIcon(ImDrawList* dl, ImVec2 p0, float s, ImU32 col, float /*t*/)
    {
        const float pad = s * 0.28f;

        const float x0 = floorf(p0.x + pad) + 0.5f;
        const float x1 = floorf(p0.x + s - pad) + 0.5f;
        const float y  = floorf(p0.y + s * 0.70f) + 0.5f;

        dl->AddLine(ImVec2(x0, y), ImVec2(x1, y), col, 1.0f);
    }

    static void DrawMaximizeIcon(ImDrawList* dl, ImVec2 p0, float s, ImU32 col, float /*t*/)
    {
        // Keep the box fully closed, but ensure EVERY edge is pixel-aligned
        const float pad = s * 0.30f;

        const float x0 = floorf(p0.x + pad) + 0.5f;
        const float y0 = floorf(p0.y + pad) + 0.5f;
        const float x1 = floorf(p0.x + s - pad) + 0.5f;
        const float y1 = floorf(p0.y + s - pad) + 0.5f;

        const float thick = 1.0f;

        dl->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), col, thick); // top
        dl->AddLine(ImVec2(x1, y0), ImVec2(x1, y1), col, thick); // right
        dl->AddLine(ImVec2(x1, y1), ImVec2(x0, y1), col, thick); // bottom
        dl->AddLine(ImVec2(x0, y1), ImVec2(x0, y0), col, thick); // left
    }

    static void DrawCloseIcon(ImDrawList* dl, ImVec2 p0, float s, ImU32 col, float t)
    {
        // perfectly symmetric X
        const float pad = s * 0.32f;

        const float x0 = floorf(p0.x + pad) + 0.5f;
        const float y0 = floorf(p0.y + pad) + 0.5f;
        const float x1 = floorf(p0.x + s - pad) + 0.5f;
        const float y1 = floorf(p0.y + s - pad) + 0.5f;

        dl->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), col, t);
        dl->AddLine(ImVec2(x0, y1), ImVec2(x1, y0), col, t);
    }

    struct WindowBtnStyle
    {
        float w = 26.0f;
        float h = 22.0f;
        float spacing = 6.0f;
        float rightPadding = 6.0f;
    };

    static void PushMenuPopupStyle()
    {
        // Inner padding of the popup window (left/right/top/bottom)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 10.0f));

        // Padding inside each menu item row (text inset + row height)
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));

        // Spacing between items (vertical gap + horizontal spacing between label/shortcut)
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));

        // Optional: makes the popup corners slightly softer
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.0f);

        // Optional: make separators breathe a bit
        ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextPadding, ImVec2(10.0f, 6.0f));
    }

    static void PopMenuPopupStyle()
    {
        ImGui::PopStyleVar(5);
    }
} // namespace

MenuUI::MenuUI()
{
    initialized = false;
    loggerPtr   = nullptr;
    eventsPtr   = nullptr;
    keybindsPtr = nullptr;
    platformPtr = nullptr;
    appctx      = nullptr;
    modalsPtr   = nullptr;

    iconTex = 0;
    iconW = iconH = 0;

    dragging = false;
    prevMousePosX = prevMousePosY = 0;
}

bool MenuUI::initialize(Logger* _loggerPtr,
                        Platform* _platformPtr,
                        EventDispatcher* _eventsPtr,
                        ModalManager* _modalsPtr,
                        Keybinds* _keybindsPtr,
                        AppContext* ctx)
{
    if (initialized)
    {
        if (loggerPtr)
            loggerPtr->addLog(LogLevel::WARNING, "Menu UI Initialization", "Menu UI was already initialized.");
        return false;
    }

    loggerPtr   = _loggerPtr;
    eventsPtr   = _eventsPtr;
    modalsPtr   = _modalsPtr;
    keybindsPtr = _keybindsPtr;
    platformPtr = _platformPtr;
    appctx      = ctx;

    initializeIconTexture();

    initialized = true;
    return true;
}

void MenuUI::render()
{
    drawMenuBar();
    if (modalsPtr) modalsPtr->render();
}

static void DrawAppIcon(unsigned int iconTex, float barH)
{
    if (iconTex == 0) return;

    const ImGuiStyle& style = ImGui::GetStyle();

    ImGui::AlignTextToFramePadding();
    const float frameH = ImGui::GetFrameHeight();

    // icon size: stay inside menu bar line cleanly
    float iconSize = frameH - 10.0f;
    if (iconSize < 8.0f) iconSize = 8.0f;

    ImVec2 p = ImGui::GetCursorScreenPos();
    const float y = p.y + (frameH - iconSize) * 0.5f;

    ImGui::GetWindowDrawList()->AddImage(
        (ImTextureID)(intptr_t)iconTex,
        ImVec2(p.x, y),
        ImVec2(p.x + iconSize, y + iconSize));

    // Advance cursor by the frame height so the next item stays aligned
    ImGui::Dummy(ImVec2(iconSize, frameH));
    ImGui::SameLine(0.0f, style.ItemSpacing.x);
}

void MenuUI::drawMenuBar()
{
    ImGuiIO& io = ImGui::GetIO();
    const float barH = 28.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, barH));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if (ImGui::Begin("##TopBar", nullptr, flags))
    {
        if (ImGui::BeginMenuBar())
        {
            if (iconTex == 0) initializeIconTexture();
            DrawAppIcon(iconTex, barH);

            // ---- Left-side menus ----
            PushMenuPopupStyle();
            auto menu = MenuEngine::getMenuStructure();
            for (const MenuItem& item : menu) MenuUI::drawMenuItem(item);
            PopMenuPopupStyle();

            ImGui::Text(appctx->project.projectTitle.c_str());
            
            // ---- Right-side window buttons ----
            const ImGuiStyle& style = ImGui::GetStyle();
            ImDrawList* dl = ImGui::GetWindowDrawList();

            WindowBtnStyle bs;
            bs.spacing      = style.ItemSpacing.x;
            bs.rightPadding = 0.0f;

            // Match the menu bar line height so they're always vertically centered
            const float lineH = ImGui::GetFrameHeight();
            bs.h = lineH;
            bs.w = lineH;

            const float btnCount = 3.0f;
            const float buttonsWidth = btnCount * bs.w + (btnCount - 1.0f) * bs.spacing;

            // Content region max is in *local* coordinates.
            const float rightX = ImGui::GetWindowContentRegionMax().x;
            const float buttonsX0 = rightX - buttonsWidth - bs.rightPadding;

            // Drag region: from current cursor X to start of buttons.
            const float dragX0 = ImGui::GetCursorPosX();
            const float dragW  = buttonsX0 - dragX0;

            if (dragW > 8.0f)
            {
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::SetCursorPosX(dragX0);

                ImGui::InvisibleButton("##TitlebarDrag", ImVec2(dragW, lineH));

                if (ImGui::IsItemActivated())
                {
                    dragging = false;

                    bool handled = false;
                    if (platformPtr) handled = platformPtr->beginNativeWindowDrag();

                    if (!handled)
                    {
                        dragging = true;
                        if (platformPtr) platformPtr->getScreenCursorPosition(prevMousePosX, prevMousePosY);
                    }
                }

                if (dragging && ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    int cx = 0, cy = 0;
                    if (platformPtr) platformPtr->getScreenCursorPosition(cx, cy);

                    if (platformPtr) platformPtr->moveWindowPosRelative(cx - prevMousePosX, cy - prevMousePosY);

                    prevMousePosX = cx;
                    prevMousePosY = cy;
                }

                if (dragging && ImGui::IsItemDeactivated())
                    dragging = false;
            }

            // Put cursor exactly where the buttons should start (same line).
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::SetCursorPosX(buttonsX0);

            const ImU32 hoverGray = ImGui::ColorConvertFloat4ToU32(ImVec4(0.25f, 0.25f, 0.30f, 1.0f));
            const ImU32 hoverRed  = ImGui::ColorConvertFloat4ToU32(ImVec4(0.85f, 0.20f, 0.20f, 1.0f));
            const ImU32 glyphCol  = IM_COL32(255, 255, 255, 255);

            auto WindowButton = [&](const char* id,
                                    ImU32 hoverBg,
                                    void (*iconFn)(ImDrawList*, ImVec2, float, ImU32, float),
                                    const std::function<void()>& onClick)
            {
                const ImVec2 p = ImGui::GetCursorScreenPos();

                if (ImGui::InvisibleButton(id, ImVec2(bs.w, bs.h)))
                    onClick();

                if (ImGui::IsItemHovered())
                    dl->AddRectFilled(p, ImVec2(p.x + bs.w, p.y + bs.h), hoverBg);

                // Draw icon centered in the button rect
                const float iconScale = 0.6f;
                const float sBase = (bs.w < bs.h) ? bs.w : bs.h;
                const float s = sBase * iconScale;

                const float gx = p.x + (bs.w - s) * 0.5f;
                const float gy = p.y + (bs.h - s) * 0.5f;

                float t = 1.0f;

                iconFn(dl, ImVec2(gx, gy), s, glyphCol, t);
            };

            // Minimize
            WindowButton("##minimize", hoverGray, &DrawMinimizeIcon, [&]
            {
                if (platformPtr) platformPtr->iconifyWindow();
            });

            ImGui::SameLine(0.0f, bs.spacing);

            // Maximize (now centered)
            WindowButton("##maximize", hoverGray, &DrawMaximizeIcon, [&]
            {
                if (platformPtr) platformPtr->maximizeWindow();
            });

            ImGui::SameLine(0.0f, bs.spacing);

            // Close (now symmetric X)
            WindowButton("##close", hoverRed, &DrawCloseIcon, [&]
            {
                if (appctx) appctx->shouldClose = true;
            });

            ImGui::EndMenuBar();
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void MenuUI::drawMenuItem(const MenuItem& item)
{
    if (item.isSubMenu())
    {
        if (ImGui::BeginMenu(item.name.data()))
        {
            for (const MenuItem& child : item.children)
                MenuUI::drawMenuItem(child);
            ImGui::EndMenu();
        }
        return;
    }

    if (item.isSeparator)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        return;
    }

    if (item.opensModal)
    {
        if (ImGui::MenuItem(item.name.data()))
        {
            if (modalsPtr) modalsPtr->open(item.modalName);
        }
        return;
    }

    if (item.noAction)
    {
        if (ImGui::MenuItem(item.name.data()))
        {
            if (eventsPtr) eventsPtr->TriggerEvent(Event{ item.eventType, false, std::monostate{} });
        }
        return;
    }

    // Normal menu item with keybind label
    const char* shortcut = "";
    std::string shortcutStr;
    if (keybindsPtr)
    {
        shortcutStr = keybindsPtr->getKeyComboStringFromAction(item.action);
        shortcut = shortcutStr.c_str();
    }

    if (ImGui::MenuItem(item.name.data(), shortcut))
    {
        if (eventsPtr) eventsPtr->TriggerEvent(Event{ item.eventType, false, std::monostate{} });
    }
}

bool MenuUI::initializeIconTexture()
{
    if (iconTex != 0) return true;

    int w = 0, h = 0, channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* pixels = stbi_load("../assets/icon.png", &w, &h, &channels, 4);
    if (!pixels) return false;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    iconTex = (unsigned int)tex;
    iconW = w;
    iconH = h;
    return true;
}