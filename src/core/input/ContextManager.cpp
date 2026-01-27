#include "core/input/ContextManager.hpp"

ControlCtx ContextManager::current_ = ControlCtx::None;
std::vector<ControlCtx> ContextManager::stack_{};

bool ContextManager::initialize() {
    ContextManager::push(ControlCtx::Editor);
    ContextManager::clearStack();
    return true;
}

void ContextManager::toggleCtx() {
    if (isEditor()) {
        push(ControlCtx::Camera);
        return;
    }
    push(ControlCtx::Editor);
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