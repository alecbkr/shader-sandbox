#include "core/MenuEngine.hpp"
#include <array>
#include "core/ui/modals/SettingsModal.hpp"

MenuItem::MenuItem(std::string_view _name, EventType _eventType) : name(_name), eventType(_eventType) { noAction = true; };
MenuItem::MenuItem(std::string_view _name, Action _action, EventType _eventType) : name(_name), action(_action), eventType(_eventType) {};
MenuItem::MenuItem(bool _isSeparator) : isSeparator(_isSeparator) {};
MenuItem::MenuItem(std::string_view _name, std::string_view _modalName, bool _opensModal) : name(_name), modalName(_modalName), opensModal(_opensModal) {};
MenuItem::MenuItem(std::string_view _name, std::span<const MenuItem> _children) : name(_name), children(_children) {};

static const std::array<MenuItem, 8> fileMenu = {{
    {"New Shader File", Action::NewShaderFile, EventType::NewFile},
    {true},
    {"Save Active Shader File", Action::SaveActiveShaderFile, EventType::SaveActiveShaderFile},
    {"Save Project", Action::SaveProject, EventType::SaveProject},
    {true},
    {"Settings", SettingsModal::ID, true},
    {true},
    {"Quit", Action::QuitApplication, EventType::Quit},
}};

static const std::array<MenuItem, 4> editMenu = {{
    {"Undo", Action::Undo, EventType::NoType},
    {"Redo", Action::Redo, EventType::NoType},
    {true},
    {"Format Active Shader", Action::FormatActiveShader, EventType::NoType},
}};

static const std::array<MenuItem, 4> togglePanelsMenu = {{
    {"Console", EventType::NoType},
    {"Editor", EventType::NoType},
    {"Inspector", EventType::NoType},
    {"Viewport", EventType::NoType},
}};

static const std::array<MenuItem, 4> viewMenu = {{
    {"Toggle Panels", std::span<const MenuItem>(togglePanelsMenu)},
    {"Reset Layout", EventType::NoType},
    {true},
    {"Fullscreen Viewport", Action::FullscreenViewport, EventType::NoType},
}};

static const std::array<MenuItem, 1> toolsMenu = {{
    {"Screenshot", Action::ScreenshotViewport, EventType::NoType},
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
