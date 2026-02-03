#pragma once
#include <string>
#include <unordered_map>

#include "EventTypes.hpp"

class Logger;
class EventDispatcher;
class Platform;

enum FileState {
    NONE,
    RENAME,
    DELETE,
    NEW
};

struct ShaderFile {
    ShaderFile(std::string filePath, std::string fileName);
    FileState state;
    std::string filePath;
    std::string fileName;
    std::string renameBuffer;
};

class FileRegistry {
public:
    FileRegistry();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, Platform* _platformPtr);
    void reloadMap();
    std::unordered_map<std::string, ShaderFile *> getFiles();

private:
    std::unordered_map<std::string, ShaderFile*> files;
    bool initialized = false;
    EventDispatcher* eventsPtr = nullptr;
    Logger* loggerPtr = nullptr;
    Platform* platformPtr = nullptr;
    bool renameFile(const EventPayload& payload);
    bool deleteFile(const EventPayload& payload);
};

