#pragma once
#include <string>
#include <vector>
#include "core/logging/Logger.hpp"

// Command interface 
class Command {
    public: 
    virtual ~Command() {}
    virtual void Execute(const std::vector<std::string>& args) const = 0; 
}; 


class HelpCommand: public Command {
    public: 
    void Execute(const std::vector<std::string>& args) const override; 
    private: 
}; 

// Clear the console log 
class ClearCommand: public Command {
    public: 
    void Execute(const std::vector<std::string>& args) const override; 
    private: 
}; 

// TODO: maybe add commands to modify env vars like bg scene color or tweak base uniforms like time