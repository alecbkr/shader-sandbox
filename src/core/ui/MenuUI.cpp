#include "core/ui/MenuUI.hpp"
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"

bool testMenuUIQuit(const EventPayload& payload) {
    printf("Testing Quit\n");
    return true;
}

bool testMenuUISave(const EventPayload& payload) {
    printf("Testing Save\n");
    return true;
}

MenuUI::MenuUI() {
    initialized = false;
    loggerPtr = nullptr;
    eventsPtr = nullptr;
}

bool MenuUI::initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Menu UI Initialization", "Menu UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    eventsPtr = _eventsPtr;

    eventsPtr->Subscribe(EventType::SaveActiveShaderFile, testMenuUISave);
    eventsPtr->Subscribe(EventType::Quit, testMenuUIQuit);
    initialized = true;
    return true;
}

void MenuUI::render() {
    drawMenuBar();
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
    } else {
        if (item.isSeparator) {
            ImGui::Separator();
        } else {
            if (ImGui::MenuItem(item.name.data(), item.shortcut.data())) {
                eventsPtr->TriggerEvent(Event{ item.eventType, false, std::monostate{} });
            }
        }
    }
}
