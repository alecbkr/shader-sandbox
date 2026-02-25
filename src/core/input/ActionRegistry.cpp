#include "core/input/ActionRegistry.hpp"
#include "core/logging/Logger.hpp"
#include <iostream>

ActionRegistry::ActionRegistry() {
    initialized = false;
    actionsToProcess.clear();
    loggerPtr = nullptr;
}

bool ActionRegistry::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Action Registry Initialization", "Action Registry was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;

    auto* log = loggerPtr;
    bind(Action::None, [log] {
        log->addLog(LogLevel::WARNING, "Action::None Callback", "The placeholder action 'None' was called.");
    });

    initialized = true;
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
    ActionRegistry::actionsToProcess.push_back(action);
}
