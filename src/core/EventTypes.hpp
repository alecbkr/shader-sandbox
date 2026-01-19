#pragma once

#include <variant>
#include <string>

enum EventType {
    // Fill out event types here
    NoType,
    WindowResize,
    KeyPressed,
    SaveActiveShaderFile,
    SaveProject,
    Quit,
    OpenFile,
    NewFile,
};

struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };
struct OpenFilePayload { std::string filePath; std::string fileName; };

using EventPayload = std::variant<
    std::monostate,
    WindowResizePayload,
    KeyPressedPayload,
    OpenFilePayload
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
    return { Quit, false, std::monostate{} };
};

inline Event OpenFileEvent(std::string filePath, std::string fileName) {
    return { OpenFile, false, OpenFilePayload{filePath, fileName} };
}

inline Event NewFileEvent() {
    return {NewFile, false, std::monostate{} };
}