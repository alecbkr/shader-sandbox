#include "ConsoleEngine.hpp"

ConsoleEngine::ConsoleEngine() {
    ConsoleEngine::registerCommand("clear", std::make_shared<ClearCommand>()); 
}

void ConsoleEngine::registerCommand(const std::string& name, std::shared_ptr<Command> command) {
    commandRegistry[name] = command; 
}
