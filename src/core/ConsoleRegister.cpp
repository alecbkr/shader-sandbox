#include "ConsoleRegister.hpp"

void ClearCommand::Execute(const std::vector<std::string>& arg) const {
    Logger::addLog(LogLevel::INFO, "Console", "Clear Console Test");
}
