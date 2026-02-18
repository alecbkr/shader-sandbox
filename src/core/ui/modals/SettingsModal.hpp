#pragma once
#include "core/ui/modals/IModal.hpp"
#include <string>
#include <vector>
#include <types.hpp>

class Logger;
class InputState;
class Keybinds;
struct AppSettings;

enum class SettingsPage {
    Keybinds
};

struct KeybindCapture {
    bool active = false;
    std::string bindingName;
    std::vector<u16> keysDraft;
};

class SettingsModal final : public IModal {
public:
    SettingsModal() = default;

    bool initialize(Logger* logger, InputState* inputs, Keybinds* keybinds, AppSettings* settings);
    static constexpr const char* ID = "settings_modal";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Logger* loggerPtr = nullptr;
    InputState* inputsPtr = nullptr;
    Keybinds* keybindsPtr = nullptr;
    AppSettings* settingsPtr = nullptr;
    
    // UI state (persist between frames)
    bool initialized = false;

    SettingsPage page = SettingsPage::Keybinds;
    KeybindCapture capture;

    void updateCaptureFromInput();
    std::string formatKeys(const std::vector<u16>& keys) const;
    void drawKeybindsPage();

    void syncFromSettings();
    void applyToSettings();
};