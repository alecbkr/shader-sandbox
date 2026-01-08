#include "ConsoleRegister.hpp"


void HelpCommand::Execute(const std::vector<std::string>& arg) const {
    // Add a string to the logging buffer 
    Logger::addLog(LogLevel::INFO, "Console", "-----Available Commands-----", -1);
    Logger::addLog(LogLevel::INFO, "Console", "  - help: gives a list of available commands", -1);
    Logger::addLog(LogLevel::INFO, "Console", "  - clear: removes all console output", -1); 
} 

void ClearCommand::Execute(const std::vector<std::string>& arg) const {
    Logger::addLog(LogLevel::INFO, "Console", "Clear Console Test", -1);
}
