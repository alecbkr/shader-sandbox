#pragma once
#include <string_view>
#include <span>

#include "core/EventTypes.hpp"

struct MenuItem {
    std::string_view name;
    std::string_view shortcut = {};
    bool isSeparator;
    std::span<const MenuItem> children = {};
    EventType eventType = EventType::NoType;
    constexpr bool isSubMenu() const { return !children.empty(); }
};

class MenuEngine {
public:
    static std::span<const MenuItem> getMenuStructure();
};
