#include "core/ui/MenuUI.hpp"
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/Keybinds.hpp"

MenuUI::MenuUI() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
    keybindsPtr = nullptr;
}

bool MenuUI::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModalManager* _modalsPtr, Keybinds* _keybindsPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Menu UI Initialization", "Menu UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;
    modalsPtr = _modalsPtr;
    keybindsPtr = _keybindsPtr;

    initialized = true;
    return true;
}

void MenuUI::render() {
    drawMenuBar();
    modalsPtr->render();
}

void MenuUI::drawMenuBar() {
    if (!ImGui::BeginMainMenuBar()) return;

    auto menu = MenuEngine::getMenuStructure();
    for (const MenuItem& item : menu) {
        MenuUI::drawMenuItem(item);
    }

    ImGui::EndMainMenuBar();
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
