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

static constexpr std::array<MenuItem, 3> fileMenu = {{
    {"Save Active Shader File", "Ctrl+S", false, {}, EventType::SaveActiveShaderFile},
    {"New Shader File", "Ctrl+N", false, {}, EventType::NewFile},
    {"Quit", "Alt+F4", false, {}, EventType::Quit},
}};

static constexpr std::array<MenuItem, 2> editMenu = {{
    {"Undo", "Ctrl+Z", false, {}, EventType::NoType},
    {"Paste", "Ctrl+V", false, {}, EventType::NoType},
}};

static constexpr std::array<MenuItem, 2> rootMenu = {{
    {"File", "", false, std::span<const MenuItem>(fileMenu), EventType::NoType},
    {"Edit", "", false, std::span<const MenuItem>(editMenu), EventType::NoType},
}};

std::span<const MenuItem> MenuEngine::getMenuStructure() {
    return std::span<const MenuItem>(rootMenu);
}
