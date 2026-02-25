#pragma once

#include <vector>
#include <cstdint>

class Logger;
class ActionRegistry;

// Make the ControlCtx a bitmask if more contexts are added
// Then make the actual application context an enum
enum class ControlCtx : std::uint8_t {
    None = 0,
    Editor,
    Camera,
    EditorCamera
};

class ContextManager {
public:
    ContextManager();
    bool initialize(Logger* _loggerPtr, ActionRegistry* _actionRegPtr);
    void toggleCtx();
    void set(ControlCtx ctx);
    ControlCtx current();
    bool is(ControlCtx ctx);
    bool isEditor();
    bool isCamera();
    void push(ControlCtx ctx);
    void pop();
    void clearStack();

private:
    bool initialized = false;
    ControlCtx current_ = ControlCtx::None;
    std::vector<ControlCtx> stack_;
    Logger* loggerPtr = nullptr;
};