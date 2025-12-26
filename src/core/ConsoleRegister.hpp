#pragma once
#include <string>

// Command interface 
class Command {
    public: 
    virtual ~Command() {}
    virtual void Execute() const = 0; 
}; 


class HelpCommand: public Command {
    public: 
    explicit HelpCommand(std::string payLoad); 
    void Execute() const override; 
        // TODO: add command to console logger 

    private: 
    std::string payLoad; 
}; 

// Clear the console log 
class ClearCommand: public Command {
    public: 
    explicit ClearCommand(std::string payLoad); 
    void Execute() const override; 
        // TODO: use clear method after implementing the console logger 
    private: 
    std::string payLoad; 
}; 

// TODO: maybe add commands to modify env vars like bg scene color or tweak base uniforms like time