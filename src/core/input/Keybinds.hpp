#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

#include "platform/components/Keys.hpp"
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
    static bool initialize();
    static Binding makeBinding(Action action, KeyCombo combo, ControlCtx ctx, Trigger trigger = Trigger::Pressed, bool enabled = true);
    static void setBindings(const std::vector<Binding>& b);
    static void addBinding(const Binding& b);
    static void clear();
    static const std::vector<Binding>& bindings();
    static bool comboDown(const KeyCombo& combo);
    static bool comboPressedThisFrame(const KeyCombo& combo);
    static void gatherActionsForFrame(ControlCtx context);

private:
    static std::vector<Binding> bindings_;
};