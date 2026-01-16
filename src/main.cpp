// ENGINE
#include "application/Application.hpp"

int main() {
    if (!Application::initialize({1280, 720, "Shader Sandbox", LoggerInitialization::CONSOLE_FILE_STDOUT})) {
        return 1;
    }
    Application::runLoop();
    return 0;
}
