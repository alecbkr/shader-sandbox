#include "core/ui/MenuUI.hpp"
#include "core/EventDispatcher.hpp"

bool MenuUI::initialized = false;

bool testMenuUIQuit(const EventPayload& payload) {
    printf("Testing Quit\n");
    return true;
}

bool testMenuUISave(const EventPayload& payload) {
    printf("Testing Save\n");
    return true;
}

bool MenuUI::initialize() {
    EventDispatcher::Subscribe(EventType::SaveActiveShaderFile, testMenuUISave);
    EventDispatcher::Subscribe(EventType::Quit, testMenuUIQuit);
    MenuUI::initialized = true;
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
                EventDispatcher::TriggerEvent(Event{ item.eventType, false, std::monostate{} });
            }
        }
    }
}
