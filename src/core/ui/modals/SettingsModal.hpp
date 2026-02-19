#pragma once
#include "core/ui/modals/IModal.hpp"
#include <string>
#include <vector>
#include <types.hpp>

class Logger;
class InputState;
class Keybinds;
class Platform;
struct AppSettings;

enum class SettingsPage {
    Keybinds,
    Styles,
    Graphics
};

struct KeybindCapture {
    bool active = false;
    std::string bindingName;
    std::vector<u16> keysDraft;
};

class SettingsModal final : public IModal {
public:
    SettingsModal() = default;

    bool initialize(Logger* logger, InputState* inputs, Keybinds* keybinds, Platform* platform, AppSettings* settings);
    static constexpr const char* ID = "Settings";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Logger* loggerPtr = nullptr;
    InputState* inputsPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    Platform* platformPtr = nullptr;
    AppSettings* settingsPtr = nullptr;
    
    // UI state (persist between frames)
    bool initialized = false;

    SettingsPage page = SettingsPage::Keybinds;
    KeybindCapture capture;

    int selectedStyleColor = 0;

    void updateCaptureFromInput();
    std::string formatKeys(const std::vector<u16>& keys) const;
    void drawKeybindsPage();
    void drawStylesPage();
    void drawGraphicsPage();

    void syncFromSettings();
    void applyToSettings();
};