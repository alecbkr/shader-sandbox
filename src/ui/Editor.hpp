#ifndef EDITOR_HPP
#define EDITOR_HPP
#include <string>


class Editor {
public:
    char* inputTextBuffer;
    unsigned int bufferSize;
    unsigned int width;
    unsigned int height;
    Editor(unsigned int bufferSize, unsigned int width, unsigned int height);
    void render();
    void destroy();
    std::string getFileContents(const char *filename);
};


#endif