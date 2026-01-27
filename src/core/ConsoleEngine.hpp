#pragma once
#include "ConsoleRegister.hpp"
#include <sstream>
#include <string> 
#include <unordered_map>
#include <memory>

class ConsoleEngine {
    public:

    ConsoleEngine(); 
    ~ConsoleEngine() = default; 
    // handle copying 
    ConsoleEngine(ConsoleEngine &other) = delete; 
    void operator=(const ConsoleEngine &) = delete; 

    void registerCommand(const std::string& name, std::shared_ptr<Command> command); 
    void processInput(const std::string& input); 

    private:
    std::unordered_map<std::string, std::shared_ptr<Command>> commandRegistry; 
    std::vector<std::string> tokenize(const std::string& input); 
}; 