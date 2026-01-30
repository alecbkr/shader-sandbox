#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

#include "platform/components/Keys.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/InputState.hpp"
#include "core/input/ContextManager.hpp"

enum class Trigger : uint8_t {
    Pressed,
    Released,
    Down
};

struct KeyCombo {
    std::vector<Key> keys;

    KeyCombo() = default;
    KeyCombo(std::initializer_list<Key> list);

    // normalize order to avoid duplicates like {S,L} vs {L,S}
    void normalize();
};

struct Binding {
    Action action = Action::None;
    KeyCombo combo;
    ControlCtx context = ControlCtx::None;
    Trigger trigger = Trigger::Pressed;
    bool enabled = true;
};

class Keybinds {
public:
    Keybinds();
    bool initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, ActionRegistry* actionRegPtr);
    Binding makeBinding(Action action, KeyCombo combo, ControlCtx ctx, Trigger trigger = Trigger::Pressed, bool enabled = true);
    void setBindings(const std::vector<Binding>& b);
    void addBinding(const Binding& b);
    void clear();
    const std::vector<Binding>& bindings();
    bool comboDown(const KeyCombo& combo);
    bool comboPressedThisFrame(const KeyCombo& combo);
    void gatherActionsForFrame(ControlCtx context);

private:
    bool initialized = false;
    std::vector<Binding> bindings_;
    Logger* loggerPtr = nullptr;
    ContextManager* ctxManagerPtr = nullptr;
    ActionRegistry* actionRegPtr = nullptr;
};