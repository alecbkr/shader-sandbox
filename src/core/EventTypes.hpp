#pragma once

#include <variant>
#include <string>

enum class EventType {
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

struct SaveActiveShaderFilePayload { std::string filePath; unsigned int modelID; };
struct ReloadShaderPayload { std::string programName; };
struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };
struct OpenFilePayload { std::string filePath; std::string fileName; unsigned int modelID; };
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

inline Event MakeSaveActiveShaderFileEvent(std::string filePath, unsigned int modelID) {
    return { EventType::SaveActiveShaderFile, false, SaveActiveShaderFilePayload{filePath, modelID} };
};

inline Event MakeReloadShaderEvent(std::string programName) {
    return { EventType::ReloadShader, false, ReloadShaderPayload{programName} };
};

inline Event MakeWindowResizeEvent(int w, int h) {
    return { EventType::WindowResize, false, WindowResizePayload{w, h} };
};

inline Event MakeKeyPressedEvent(int key) {
    return { EventType::KeyPressed, false, KeyPressedPayload{key} };
};

inline Event MakeQuitAppEvent() {
    return { EventType::Quit, false, std::monostate{} };
};

inline Event OpenFileEvent(std::string filePath, std::string fileName, unsigned int modelID) {
    return { EventType::OpenFile, false, OpenFilePayload{filePath, fileName, modelID} };
}

inline Event NewFileEvent() {
    return { EventType::NewFile, false, std::monostate{} };
}

inline Event RenameFileEvent(std::string oldName, std::string newName) {
    return { EventType::RenameFile, false, RenameFilePayload{oldName, newName} };
}

inline Event DeleteFileEvent(std::string fileName) {
    return { EventType::DeleteFile, false, DeleteFilePayload{fileName} };
}