#include "ConsoleEngine.hpp"

bool ConsoleEngine::initialized = false;
std::shared_ptr<ConsoleSink> ConsoleEngine::logSrc = nullptr;
std::unordered_map<std::string, std::function<void()>> ConsoleEngine::btnRegistry;
std::unordered_map<std::string, std::function<void(bool)>> ConsoleEngine::toggleRegistry; 

bool ConsoleEngine::initialize(std::shared_ptr<ConsoleSink> consoleSink)
{
    logSrc = consoleSink;

    registerButton(Cmd::CLEAR, [](){logSrc->clearLogs();}); 
    initialized = true;
    return true;
}

// updates the action registry given a new name and action
void ConsoleEngine::registerButton(const std::string &name, std::function<void()> callbackFn)
{
    btnRegistry[name] = callbackFn; 
}

void ConsoleEngine::registerToggle(const std::string &name, std::function<void(bool)> callbackFn) {
    toggleRegistry[name] = callbackFn; 
}

const std::deque<LogEntry> &ConsoleEngine::getLogs()
{
    return logSrc->getLogs();
}

void ConsoleEngine::executeBtnAction(const std::string &name)
{
    if(btnRegistry.count(name)) {
        btnRegistry[name](); 
    }
}
