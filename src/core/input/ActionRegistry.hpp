#pragma once

#include <array>
#include <vector>
#include <functional>

class Logger;
using ActionFn = std::function<void()>;

enum class Action {
    None,
    CameraForward,
    CameraBack,
    CameraLeft,
    CameraRight,
    CamearUp,
    CameraDown,
    SwitchControlContext,
    SaveActiveShaderFile,
    SaveProject,
    QuitApplication,
    Count
};

class ActionRegistry {
public:
    ActionRegistry();
    bool initialize(Logger* _loggerPtr);
    void bind(Action action, ActionFn fn);
    void trigger(Action action);
    void processActionsForFrame();
    void addActionToProcess(Action action);
private:
    bool initialized = false;
    std::array<ActionFn, (std::size_t)Action::Count> actions;
    std::vector<Action> actionsToProcess;
    Logger* loggerPtr = nullptr;
};