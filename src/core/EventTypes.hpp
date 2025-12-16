#pragma once

#include <variant>

enum EventType {
    // Fill out event types here
    WindowResize,
    KeyPressed,
    QuitApp,
};

struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };

using EventPayload = std::variant<
    std::monostate,
    WindowResizePayload,
    KeyPressedPayload
>;

struct Event {
    EventType type;
    bool handled = false;
    EventPayload payload;
};

inline Event MakeWindowResizeEvent(int w, int h) {
    return { WindowResize, false, WindowResizePayload{w, h} };
};

inline Event MakeKeyPressedEvent(int key) {
    return { KeyPressed, false, KeyPressedPayload{key} };
};

inline Event MakeQuitAppEvent() {
    return { QuitApp, false, std::monostate{} };
};