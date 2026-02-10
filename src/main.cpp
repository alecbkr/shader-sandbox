#include "application/Application.hpp"
#include "core/logging/Logger.hpp"

#define START_WIDTH 960
#define START_HEIGHT 540
#define APPLICATION_TITLE "Shader Sandbox"

int main() {
    AppContext ctx = AppContext(START_WIDTH, START_HEIGHT, APPLICATION_TITLE);
    if (!Application::initialize(ctx)) {
        return 1;
    }
    Application::runLoop(ctx);
    return 0;
}
