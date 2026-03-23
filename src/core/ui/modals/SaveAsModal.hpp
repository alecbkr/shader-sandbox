#pragma once
#include "IModal.hpp"
#include "application/AppSettings.hpp"
#include "application/Project.hpp"
#include "core/EventDispatcher.hpp"
#include "core/logging/Logger.hpp"

class SaveAsModal final : public IModal {
public:
    SaveAsModal() = default;

    bool initialize(Logger* logger, Project* project, EventDispatcher* events, AppSettings* settings, bool* projectSwitch);
    static constexpr const char* ID = "Save Project As";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Logger* loggerPtr = nullptr;
    Project* projectPtr = nullptr;
    EventDispatcher* eventPtr = nullptr;
    AppSettings* settingsPtr = nullptr;
    bool* projectSwitchPtr = nullptr;
    bool initialized = false;
    char inputBuffer[512] = "";
    std::string findNextFileNumber(const std::string& startingName);
};
