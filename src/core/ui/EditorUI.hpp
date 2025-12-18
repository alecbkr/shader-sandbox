#pragma once

class EditorUI {
public:
    char* inputTextBuffer;
    unsigned int bufferSize;
    int previousTextLen;
    int lineCount;
    EditorUI(unsigned int bufferSize);
    void render();
    void destroy();
};
