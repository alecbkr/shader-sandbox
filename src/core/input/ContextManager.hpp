#pragma once

#include <vector>

// Make the ControlCtx a bitmask if more contexts are added
// Then make the actual application context an enum
enum class ControlCtx {
    None = 0,
    Editor,
    Camera,
    EditorCamera
};

class ContextManager {
public:
    static bool initialize();
    static void toggleCtx();
    static void set(ControlCtx ctx);
    static ControlCtx current();
    static bool is(ControlCtx ctx);
    static bool isEditor();
    static bool isCamera();
    static void push(ControlCtx ctx);
    static void pop();
    static void clearStack();

private:
    static ControlCtx current_;
    static std::vector<ControlCtx> stack_;
};