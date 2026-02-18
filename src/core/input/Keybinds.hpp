#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <types.hpp>
#include <string>
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "platform/components/Keys.hpp"

class Logger;
class InputState;
struct SettingsKeybind;

enum class Trigger : uint8_t {
    Pressed,
    Released,
    Down
};

struct KeyCombo {
    std::vector<Key> keys;

    KeyCombo() = default;
    KeyCombo(std::vector<u16> _keys);
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
    bool initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, ActionRegistry* _actionRegPtr, InputState* _inputsPtr, const std::unordered_map<std::string, SettingsKeybind>& keybindsMap);
    void shutdown();
    Binding makeBinding(Action action, KeyCombo combo, ControlCtx ctx, Trigger trigger = Trigger::Pressed, bool enabled = true);
    void syncBindings(const std::unordered_map<std::string, SettingsKeybind>& keybindsMap);
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
    InputState* inputsPtr = nullptr;
};