#pragma once

#include <array>
#include <vector>

using ActionFn = void(*)();

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
    static bool initialize();
    static void bind(Action action, ActionFn fn);
    static void trigger(Action action);
    static void processActionsForFrame();
    static void addActionToProcess(Action action);
private:
    static std::array<ActionFn, (std::size_t)Action::Count> actions;
    static std::vector<Action> actionsToProcess;
};