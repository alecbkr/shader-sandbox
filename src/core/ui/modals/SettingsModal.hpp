#pragma once
#include "core/ui/modals/IModal.hpp"
#include <string>
#include <vector>
#include <types.hpp>
#include <filesystem>

class Logger;
class InputState;
class Keybinds;
class Platform;
class Project;
class EventDispatcher;
struct AppSettings;

enum class SettingsPage {
    Keybinds,
    Styles,
    Graphics,
    Folders
};

struct KeybindCapture {
    bool active = false;
    std::string bindingName;
    std::vector<u16> keysDraft;
};

class SettingsModal final : public IModal {
public:
    SettingsModal() = default;

    bool initialize(Logger* logger, InputState* inputs, Keybinds* keybinds, Platform* platform, AppSettings* settings, Project* project, EventDispatcher* events);
    static constexpr const char* ID = "Settings";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Logger* loggerPtr = nullptr;
    InputState* inputsPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    Platform* platformPtr = nullptr;
    AppSettings* settingsPtr = nullptr;
    Project* projectPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    
    // UI state (persist between frames)
    bool initialized = false;

    SettingsPage page = SettingsPage::Keybinds;
    KeybindCapture capture;

    int selectedTheme = 0;
    int selectedStyleColor = 0;
    int selectedEditorStyleColor = 0;
    int selectedInspectorStyleColor = 0;
    int selectedConsoleStyleColor = 0;

    void updateCaptureFromInput();
    std::string formatKeys(const std::vector<u16>& keys) const;
    void drawKeybindsPage();
    void drawStylesPage();
    void drawGraphicsPage();
    void drawFoldersPage();

    void syncFromSettings();
    void applyToSettings();
    void openFolder(const std::filesystem::path& path);
};