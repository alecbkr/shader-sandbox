#include "core/input/Keybinds.hpp"
#include "core/logging/Logger.hpp"
#include "core/input/InputState.hpp"
#include "application/AppSettings.hpp"

KeyCombo::KeyCombo(std::vector<u16> _keys) {
    keys.reserve(_keys.size());
    for (u16 key : _keys) keys.push_back((Key)key);
    normalize();
}

KeyCombo::KeyCombo(std::initializer_list<Key> list) : keys(list) {
    normalize();
}

void KeyCombo::normalize() {
    std::sort(keys.begin(), keys.end(), [](Key a, Key b){
        return keyIndex(a) < keyIndex(b);
    });
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
}

Keybinds::Keybinds() {
    initialized = false;
    bindings_.clear();
    loggerPtr = nullptr;
    ctxManagerPtr = nullptr;
    actionRegPtr = nullptr;
    inputsPtr = nullptr;
}

bool Keybinds::initialize(Logger* _loggerPtr, ContextManager* _ctxManagerPtr, ActionRegistry* _actionRegPtr, InputState* _inputsPtr, const std::unordered_map<std::string, SettingsKeybind>& keybindsMap) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Keybinds Initialization", "Keybinds have already been initialzied.");
        return false;
    }
    loggerPtr = _loggerPtr;
    ctxManagerPtr = _ctxManagerPtr;
    actionRegPtr = _actionRegPtr;
    inputsPtr = _inputsPtr;
    
    syncBindings(keybindsMap);
    
    initialized = true;
    return true;
}

void Keybinds::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    ctxManagerPtr = nullptr;
    actionRegPtr = nullptr;
    inputsPtr = nullptr;
    bindings_.clear();
    initialized = false;
}

Binding Keybinds::makeBinding(Action action, KeyCombo combo, ControlCtx ctx, Trigger trigger, bool enabled) {
    return Binding{action, combo, ctx, trigger, enabled};
}

void Keybinds::syncBindings(const std::unordered_map<std::string, SettingsKeybind>& keybindsMap) {
    bindings_.clear();
    for (const auto& [name, bind] : keybindsMap) {
        addBinding(makeBinding((Action)bind.action, KeyCombo(bind.keys), (ControlCtx)bind.context, (Trigger)bind.trigger));
    }
}

void Keybinds::setBindings(const std::vector<Binding>& b) {
    bindings_ = b;
}

void Keybinds::addBinding(const Binding& b) {
    bindings_.push_back(b);
}

void Keybinds::clear() {
    bindings_.clear(); 
}

const std::vector<Binding>& Keybinds::bindings() {
    return bindings_;
}

bool Keybinds::comboDown(const KeyCombo& combo) {
    if (combo.keys.empty()) return false;
    for (Key key : combo.keys) {
        if (!inputsPtr->isDownKey(key)) return false;
    }
    return true;
}

bool Keybinds::comboPressedThisFrame(const KeyCombo& combo) {
    if (!comboDown(combo)) return false;

    for (Key key : combo.keys) {
        if (inputsPtr->wasPressed(key)) return true;
    }
    return false;
}

void Keybinds::gatherActionsForFrame(ControlCtx context) {

    for (const Binding& binding : bindings_) {
        if (!binding.enabled) continue;
        if (binding.action == Action::SaveActiveShaderFile && context == ControlCtx::Camera) {
            int test = 1 + 1;
        }
        if ((binding.context != ControlCtx::EditorCamera)) {
            if ((binding.context != context)) continue;
        }

        if (binding.trigger == Trigger::Down) {
            if (comboDown(binding.combo)) actionRegPtr->addActionToProcess(binding.action);
        } else if (binding.trigger == Trigger::Pressed) {
            if (comboPressedThisFrame(binding.combo)) {
                actionRegPtr->addActionToProcess(binding.action);
            }
        }
    }
}