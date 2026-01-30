#pragma once
#include <string>
#include <unordered_map>

#include "EventTypes.hpp"
#include "EventDispatcher.hpp"
#include "logging/Logger.hpp"

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
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr);
    void reloadMap();
    std::unordered_map<std::string, ShaderFile *> getFiles();

private:
    std::unordered_map<std::string, ShaderFile*> files;
    bool initialized = false;
    EventDispatcher* eventsPtr = nullptr;
    Logger* loggerPtr = nullptr;
    bool renameFile(const EventPayload& payload);
    bool deleteFile(const EventPayload& payload);
};

