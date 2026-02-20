#include "core/ui/MenuUI.hpp"
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/Keybinds.hpp"
#include "platform/Platform.hpp"
#include "application/AppContext.hpp"

MenuUI::MenuUI() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    keybindsPtr = nullptr;
    platformPtr = nullptr;
    appctx = nullptr;
}

bool MenuUI::initialize(Logger* _loggerPtr, Platform* _platformPtr, EventDispatcher* _eventsPtr, ModalManager* _modalsPtr, Keybinds* _keybindsPtr, AppContext* ctx) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Menu UI Initialization", "Menu UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    modalsPtr = _modalsPtr;
    keybindsPtr = _keybindsPtr;
    platformPtr = _platformPtr;
    appctx = ctx;

    initialized = true;
    return true;
}

void MenuUI::render() {
    drawMenuBar();
    modalsPtr->render();
}

void MenuUI::drawMenuBar() {
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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if (ImGui::Begin("##TopBar", nullptr, flags)) {
        if (ImGui::BeginMenuBar()) {
            auto menu = MenuEngine::getMenuStructure();
            for (const MenuItem& item : menu)
                MenuUI::drawMenuItem(item);

            float btnW = 26.0f;
            float btnH = barH - 6.0f;
            float btnCount = 3.0f;
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float rightPadding = 6.0f;

            // Where buttons will start (right side)
            float right = ImGui::GetWindowContentRegionMax().x;
            float buttonsWidth = btnCount * btnW + (btnCount - 1.0f) * spacing;
            float buttonsX0 = right - buttonsWidth - rightPadding;

            // Current cursor X after menus (left side)
            float dragX0 = ImGui::GetCursorPosX();
            float dragX1 = buttonsX0;
            float dragW  = dragX1 - dragX0;

            if (dragW > 10.0f)
            {
                ImGui::SameLine();
                ImGui::SetCursorPosX(dragX0);

                // Height should match menu bar frame height
                ImVec2 dragSize(dragW, ImGui::GetFrameHeight());
                ImGui::InvisibleButton("##TitlebarDrag", dragSize);

                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    ImVec2 d = ImGui::GetIO().MouseDelta;
                    platformPtr->moveWindowPosRelative((int)d.x, (int)d.y);
                }
            }

            if (ImGui::Button("_", ImVec2(btnW, btnH)))
                platformPtr->iconifyWindow();

            ImGui::SameLine();

            if (ImGui::Button("[]", ImVec2(btnW, btnH))) {
                platformPtr->maximizeWindow();
            }

            ImGui::SameLine();

            if (ImGui::Button("X", ImVec2(btnW, btnH))) {
                appctx->shouldClose = true;
            }

            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
                !ImGui::IsAnyItemActive() &&
                ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 d = io.MouseDelta;
                platformPtr->moveWindowPosRelative((int)d.x, (int)d.y);
            }
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void MenuUI::drawMenuItem(const MenuItem& item) {
    if (item.isSubMenu()) {
        if (ImGui::BeginMenu(item.name.data())) {
            auto& submenu = item.children;
            for (const MenuItem& item : submenu) {
                MenuUI::drawMenuItem(item);
            }
            ImGui::EndMenu();
        }
    } else if (item.isSeparator) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    } else if (item.opensModal) {
        if (ImGui::MenuItem(item.name.data())) {
            if (modalsPtr) modalsPtr->open(item.modalName);
        }
    } else if (item.noAction) {
        if (ImGui::MenuItem(item.name.data())) {
            eventsPtr->TriggerEvent(Event{ item.eventType, false, std::monostate{} });
        }
    } else {
        if (ImGui::MenuItem(item.name.data(), keybindsPtr->getKeyComboStringFromAction(item.action).c_str())) {
            eventsPtr->TriggerEvent(Event{ item.eventType, false, std::monostate{} });
        }
    }
}
