#include "ConsoleEngine.hpp"

ConsoleEngine::ConsoleEngine() {
    ConsoleEngine::registerCommand("clear", std::make_shared<ClearCommand>()); 
}

void ConsoleEngine::registerCommand(const std::string& name, std::shared_ptr<Command> command) {
    commandRegistry[name] = command; 
}

void ConsoleEngine::processInput(const std::string& input) {
    std::vector<std::string> args = tokenize(input); 

    if (args.empty()) {
        return; 
    }

    // get the name of the command we need to exec 
    std::string cmdName = args[0]; 

    auto iter = commandRegistry.find(cmdName); 

    // validate that the command exists inside the hash map
    if(iter != commandRegistry.end()) {
        std::shared_ptr<Command> cmd = iter->second; 
        cmd->Execute(args); 
    } else {
        //Logger::addLog(LogLevel::LOG_ERROR, "command not found", "Could not find command '" + args[0] + "'");
    }
}

std::vector<std::string> ConsoleEngine::tokenize(const std::string& input) {
    std::istringstream iss(input); 
    std::vector<std::string> tokens; 
    std::string token; 

    // split white spaces 
    while(iss >> token) {
        tokens.push_back(token);
    }

    return tokens; 
}

