#pragma once

#include <variant>
#include <string>

enum EventType {
    // Fill out event types here
    NoType,
    WindowResize,
    KeyPressed,
    SaveActiveShaderFile,
    ReloadShader,
    SaveProject,
    Quit,
    OpenFile,
    NewFile,
    RenameFile,
    DeleteFile,
};

struct SaveActiveShaderFilePayload { std::string filePath; std::string programName; };
struct ReloadShaderPayload { std::string programName; };
struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };
struct OpenFilePayload { std::string filePath; std::string fileName; };
struct RenameFilePayload { std::string oldName, newName; };
struct DeleteFilePayload { std::string fileName; };

using EventPayload = std::variant<
    std::monostate,
    SaveActiveShaderFilePayload,
    ReloadShaderPayload,
    WindowResizePayload,
    KeyPressedPayload,
    OpenFilePayload,
    RenameFilePayload,
    DeleteFilePayload
>;

struct Event {
    EventType type;
    bool handled = false;
    EventPayload payload;
};

inline Event MakeSaveActiveShaderFileEvent(std::string filePath, std::string programName) {
    return { SaveActiveShaderFile, false, SaveActiveShaderFilePayload{filePath, programName} };
};

inline Event MakeReloadShaderEvent(std::string programName) {
    return { ReloadShader, false, ReloadShaderPayload{programName} };
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

inline Event RenameFileEvent(std::string oldName, std::string newName) {
    return { RenameFile, false, RenameFilePayload{oldName, newName} };
}

inline Event DeleteFileEvent(std::string fileName) {
    return { DeleteFile, false, DeleteFilePayload{fileName} };
}