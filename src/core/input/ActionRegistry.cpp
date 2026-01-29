#include "core/input/ActionRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/input/ContextManager.hpp"
#include "core/ui/ViewportUI.hpp"

std::array<ActionFn, (std::size_t)Action::Count> ActionRegistry::actions{};
std::vector<Action> ActionRegistry::actionsToProcess{};

void noneCB() {
    Logger::addLog(LogLevel::WARNING, "Action::None Callback", "The placeholder action 'None' was called. This means there is a keybind that was pressed that is bound to an empty action.");
}

void cameraForwardCB() { ViewportUI::getCamera()->MoveForward(); } // Move camera forward
void cameraBackCB() { ViewportUI::getCamera()->MoveBack(); } // Move camera back
void cameraLeftCB() { ViewportUI::getCamera()->MoveLeft(); } // Move camera left
void cameraRightCB() { ViewportUI::getCamera()->MoveRight(); } // Move camera right
void cameraUpCB() { ViewportUI::getCamera()->MoveUp(); } // Move camera up
void cameraDownCB() { ViewportUI::getCamera()->MoveDown(); } // Move camera down
void switchControlCtxCB() { ContextManager::toggleCtx(); }
void saveActiveShaderFileCB() { EventDispatcher::TriggerEvent({ EventType::SaveActiveShaderFile, false, std::monostate{} }); }
void saveProjectCB() { EventDispatcher::TriggerEvent({ EventType::SaveProject, false, std::monostate{} }); }
void quitApplicationCB() { EventDispatcher::TriggerEvent({ EventType::Quit, false, std::monostate{} }); }

bool ActionRegistry::initialize() {
    bind(Action::None, noneCB);
    bind(Action::CameraForward, cameraForwardCB);
    bind(Action::CameraBack, cameraBackCB);
    bind(Action::CameraLeft, cameraLeftCB);
    bind(Action::CameraRight, cameraRightCB);
    bind(Action::CamearUp, cameraUpCB);
    bind(Action::CameraDown, cameraDownCB);
    bind(Action::SwitchControlContext, switchControlCtxCB);
    bind(Action::SaveActiveShaderFile, saveActiveShaderFileCB);
    bind(Action::SaveProject, saveProjectCB);
    bind(Action::QuitApplication, quitApplicationCB);
    return true;
}

void ActionRegistry::bind(Action action, ActionFn fn) {
    actions[(std::size_t)action] = fn;
}

void ActionRegistry::trigger(Action action) {
    if (auto fn = actions[(std::size_t)action]) fn();
}

void ActionRegistry::processActionsForFrame() {
    if (ActionRegistry::actionsToProcess.empty()) return;
    for (Action action : ActionRegistry::actionsToProcess) {
        trigger(action);
    }
    ActionRegistry::actionsToProcess.clear();
}

void ActionRegistry::addActionToProcess(Action action) {
    // Logger::addLog(LogLevel::INFO, "Action Registry", "An action was recorded.");
    ActionRegistry::actionsToProcess.push_back(action);
}
