#pragma once
#include <string_view>
#include <span>

#include "core/EventTypes.hpp"
#include "core/input/ActionRegistry.hpp"

struct MenuItem {
    MenuItem() = default;
    MenuItem(std::string_view name, EventType eventType);
    MenuItem(std::string_view name, Action action, EventType eventType);
    MenuItem(bool isSeparator);
    MenuItem(std::string_view name, std::string_view modalName, bool opensModal);
    MenuItem(std::string_view name, std::span<const MenuItem> children);
    std::string_view name = {};
    Action action = Action::None;
    std::string_view modalName = {};
    bool isSeparator = false;
    bool opensModal = false;
    bool noAction = false;
    std::span<const MenuItem> children = {};
    EventType eventType = EventType::NoType;
    constexpr bool isSubMenu() const { return !children.empty(); }
};

class MenuEngine {
public:
    static std::span<const MenuItem> getMenuStructure();
};