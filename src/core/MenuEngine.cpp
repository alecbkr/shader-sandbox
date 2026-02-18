#include "core/MenuEngine.hpp"
#include <array>

MenuItem::MenuItem(std::string_view _name, EventType _eventType) : name(_name), eventType(_eventType) {};
MenuItem::MenuItem(std::string_view _name, std::string_view _shortcut, EventType _eventType) : name(_name), shortcut(_shortcut), eventType(_eventType) {};
MenuItem::MenuItem(bool _isSeparator) : isSeparator(_isSeparator) {};
MenuItem::MenuItem(std::string_view _name, std::string_view _modalName, bool _opensModal) : name(_name), modalName(_modalName), opensModal(_opensModal) {};
MenuItem::MenuItem(std::string_view _name, std::span<const MenuItem> _children) : name(_name), children(_children) {};
// Example of how a more complex menu might look.

// static constexpr std::array<MenuItem, 2> exportMenu = {{
//     {"Export Image...", "Ctrl+E"},
//     {"Export Shader..."},
// }};

// static constexpr std::array<MenuItem, 7> fileMenu = {{
//     {"New", "Ctrl+N"},
//     {"Open...", "Ctrl+O"},
//     {"", "", true}, // separator line
//     {"Export", std::span<const MenuItem>(exportMenu)},
//     {"", "", true}, // separator line
//     {"Save Active Shader File", "Ctrl+S", false},
//     {"Quit", "Alt+F4"},
// }};

static const std::array<ModalItem, 1> settingsModal = {{
    {"", ModalInput::MI_TEXT},
}};

static const std::array<MenuItem, 6> fileMenu = {{
    {"New Shader File", "Ctrl+N", EventType::NewFile},
    {"Save Active Shader File", "Ctrl+S", EventType::SaveActiveShaderFile},
    {"Save Project", "Ctrl+Shift+S", EventType::SaveProject},
    {"Reload Shader", "Ctrl+R", EventType::ReloadShader},
    {"Settings", "settings_modal", true},
    {"Quit", "Alt+F4", EventType::Quit},
}};

static const std::array<MenuItem, 5> editMenu = {{
    {"Undo", "Ctrl+Z", EventType::NoType},
    {"Redo", "Ctrl+Y", EventType::NoType},
    {"Format Shader", "Ctrl+F", EventType::NoType},
    {"Copy Selection", "Ctrl+C", EventType::NoType},
    {"Paste", "Ctrl+V", EventType::NoType},
}};

static const std::array<MenuItem, 4> togglePanelsMenu = {{
    {"Console", "", EventType::NoType},
    {"Editor", "", EventType::NoType},
    {"Inspector", "", EventType::NoType},
    {"Viewport", "", EventType::NoType},
}};

static const std::array<MenuItem, 3> viewMenu = {{
    {"Toggle Panels", std::span<const MenuItem>(togglePanelsMenu)},
    {"Reset Layout", "", EventType::NoType},
    {"Fullscreen Viewport", "", EventType::NoType},
}};

static const std::array<MenuItem, 1> toolsMenu = {{
    {"Screenshot", "", EventType::NoType},
}};

static const std::array<MenuItem, 4> rootMenu = {{
    {"File", std::span<const MenuItem>(fileMenu)},
    {"Edit", std::span<const MenuItem>(editMenu)},
    {"View", std::span<const MenuItem>(viewMenu)},
    {"Tools", std::span<const MenuItem>(toolsMenu)},
}};

std::span<const MenuItem> MenuEngine::getMenuStructure() {
    return std::span<const MenuItem>(rootMenu);
}
