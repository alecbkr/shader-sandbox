#include "core/input/ContextManager.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/ActionRegistry.hpp"

ContextManager::ContextManager() {
    current_ = ControlCtx::None;
    stack_.clear();
    loggerPtr = nullptr;
    initialized = false;
}

bool ContextManager::initialize(Logger* _loggerPtr, ActionRegistry* _actionRegistryPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Context Manager Initialize", "Context Manager is already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    current_ = ControlCtx::Editor;

    _actionRegistryPtr->bind(Action::SwitchControlContext, [&]{ toggleCtx(); });

    ContextManager::push(ControlCtx::Editor);
    ContextManager::clearStack();
    initialized = true;
    return true;
}

void ContextManager::toggleCtx() {
    if (isEditor()) {
        push(ControlCtx::Camera);
    } else {
        push(ControlCtx::Editor);
    }
}

void ContextManager::set(ControlCtx ctx) { current_ = ctx; }
ControlCtx ContextManager::current() { return current_; }
bool ContextManager::is(ControlCtx ctx) { return current_ == ctx; }
bool ContextManager::isEditor() { return current_ == ControlCtx::Editor; }
bool ContextManager::isCamera() { return current_ == ControlCtx::Camera; }

void ContextManager::push(ControlCtx ctx) {
    stack_.push_back(current_);
    current_ = ctx;
}

void ContextManager::pop() {
    if (stack_.empty()) return;
    current_ = stack_.back();
    stack_.pop_back();
}

void ContextManager::clearStack() { stack_.clear(); }