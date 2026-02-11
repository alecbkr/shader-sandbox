#include "ConsoleEngine.hpp"
#include "logging/Logger.hpp"

bool ConsoleEngine::initialize(Logger* _loggerPtr) {
    if (initialized) {
        _loggerPtr->addLog(LogLevel::WARNING, "Console UI Initialization", "Console UI was already initialized.");
        return false;
    }

    logSrc = _loggerPtr->getConsoleSinkPtr();

    // Register Buttons 
    registerButton(ConsoleActions::CLEAR, [this](){
        if (logSrc) logSrc->clearLogs();
    }); 

    // Register Toggles 
    registerToggle(ConsoleActions::AUTO_SCROLL, [this](bool state) {
        toggles.isAutoScroll = state; 
    });
    registerToggle(ConsoleActions::COLLAPSE_LOGS, [this](bool state) {
        toggles.isCollapsedLogs = state; 
    });
    registerToggle(ConsoleActions::SHOW_ERRORS, [this](bool state) {
        toggles.isShowError = state; 
    }); 
    registerToggle(ConsoleActions::SHOW_WARNINGS, [this](bool state) {
        toggles.isShowWarning = state; 
    });
    registerToggle(ConsoleActions::SHOW_INFO, [this](bool state) {
        toggles.isShowInfo = state; 
    });
    // TODO: implement other filters that filter by the log's source 
    
    initialized = true;
    return true;
}

// updates the action registry given a new name and action
void ConsoleEngine::registerButton(std::string_view name, std::function<void()> callbackFn) {
    btnRegistry[name] = std::move(callbackFn); 
}

void ConsoleEngine::registerToggle(std::string_view name, std::function<void(bool)> callbackFn) {
    toggleRegistry[name] = std::move(callbackFn); 
}

const std::deque<LogEntry>& ConsoleEngine::getLogs() const {
    return logSrc->getLogs();
}

void ConsoleEngine::executeBtnAction(std::string_view name) {
    if (auto it = btnRegistry.find(name); it != btnRegistry.end()) {
        it->second(); 
    }
}

ConsoleToggles& ConsoleEngine::getToggles() {
    return toggles; 
}