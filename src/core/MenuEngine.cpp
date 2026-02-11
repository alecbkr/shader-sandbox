#include "core/MenuEngine.hpp"
#include <array>

// Example of how a more complex menu might look.

// static constexpr std::array<MenuItem, 2> exportMenu = {{
//     {"Export Image...", "Ctrl+E"},
//     {"Export Shader..."},
// }};

// static constexpr std::array<MenuItem, 7> fileMenu = {{
//     {"New", "Ctrl+N"},
//     {"Open...", "Ctrl+O"},
//     {"", "", true}, // separator line
//     {"Export", "", false, std::span<const MenuItem>(exportMenu)},
//     {"", "", true}, // separator line
//     {"Save Active Shader File", "Ctrl+S", false},
//     {"Quit", "Alt+F4"},
// }};

static constexpr std::array<MenuItem, 6> fileMenu = {{
    {"New Shader File", "Ctrl+N", false, {}, EventType::NewFile},
    {"Save Active Shader File", "Ctrl+S", false, {}, EventType::SaveActiveShaderFile},
    {"Save Project", "Ctrl+Shift+S", false, {}, EventType::SaveProject},
    {"Reload Shader", "Ctrl+R", false, {}, EventType::ReloadShader},
    {"Settings", "", false, {}, EventType::NoType},
    {"Quit", "Alt+F4", false, {}, EventType::Quit},
}};

static constexpr std::array<MenuItem, 5> editMenu = {{
    {"Undo", "Ctrl+Z", false, {}, EventType::NoType},
    {"Redo", "Ctrl+Y", false, {}, EventType::NoType},
    {"Format Shader", "Ctrl+F", false, {}, EventType::NoType},
    {"Copy Selection", "Ctrl+C", false, {}, EventType::NoType},
    {"Paste", "Ctrl+V", false, {}, EventType::NoType},
}};

static constexpr std::array<MenuItem, 4> togglePanelsMenu = {{
    {"Console", "", false, {}, EventType::NoType},
    {"Editor", "", false, {}, EventType::NoType},
    {"Inspector", "", false, {}, EventType::NoType},
    {"Viewport", "", false, {}, EventType::NoType},
}};

static constexpr std::array<MenuItem, 3> viewMenu = {{
    {"Toggle Panels", "", false, std::span<const MenuItem>(togglePanelsMenu)},
    {"Reset Layout", "", false, {}, EventType::NoType},
    {"Fullscreen Viewport", "", false, {}, EventType::NoType},
}};

static constexpr std::array<MenuItem, 1> toolsMenu = {{
    {"Screenshot", "", false, {}, EventType::NoType},
}};

static constexpr std::array<MenuItem, 4> rootMenu = {{
    {"File", "", false, std::span<const MenuItem>(fileMenu), EventType::NoType},
    {"Edit", "", false, std::span<const MenuItem>(editMenu), EventType::NoType},
    {"View", "", false, std::span<const MenuItem>(viewMenu), EventType::NoType},
    {"Tools", "", false, std::span<const MenuItem>(toolsMenu), EventType::NoType},
}};

std::span<const MenuItem> MenuEngine::getMenuStructure() {
    return std::span<const MenuItem>(rootMenu);
}
