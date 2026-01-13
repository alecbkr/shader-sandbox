#pragma once
#include <filesystem>

class EditorUI {
public:
    char* inputTextBuffer;
    unsigned int bufferSize;
    int previousTextLen;
    int lineCount;
    std::string filePath;
    std::string fileName;
    EditorUI(unsigned int bufferSize, std::string filePath, std::string fileName);
    void render();
    void destroy();
};
