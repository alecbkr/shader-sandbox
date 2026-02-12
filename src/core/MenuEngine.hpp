#pragma once
#include <string_view>
#include <span>

#include "core/EventTypes.hpp"

enum class ModalInput {
    MI_TEXT,
    MI_DROPDOWN,
    MI_COLOR,
};

struct ModalItem {
    std::string_view name = {};
    ModalInput input = ModalInput::MI_TEXT;
};

struct Modal {
    std::span<const ModalItem> children = {};
};

struct MenuItem {
    MenuItem() = default;
    MenuItem(std::string_view name, EventType eventType);
    MenuItem(std::string_view name, std::string_view shortcut, EventType eventType);
    MenuItem(bool isSeparator);
    MenuItem(std::string_view name, std::string_view modalName, bool opensModal);
    MenuItem(std::string_view name, std::span<const MenuItem> children);
    std::string_view name = {};
    std::string_view shortcut = {};
    std::string_view modalName = {};
    bool isSeparator = false;
    bool opensModal = false;
    std::span<const MenuItem> children = {};
    Modal modal = {};
    EventType eventType = EventType::NoType;
    constexpr bool isSubMenu() const { return !children.empty(); }
};

class MenuEngine {
public:
    static std::span<const MenuItem> getMenuStructure();
};
