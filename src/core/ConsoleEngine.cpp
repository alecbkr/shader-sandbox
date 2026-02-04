#include "ConsoleEngine.hpp"
#include "logging/Logger.hpp"
bool ConsoleEngine::initialized = false;
std::shared_ptr<ConsoleSink> ConsoleEngine::logSrc = nullptr;
std::unordered_map<std::string, std::function<void()>> ConsoleEngine::btnRegistry;
std::unordered_map<std::string, std::function<void(bool)>> ConsoleEngine::toggleRegistry; 

// each name for each action for the toggles/buttons in the console 
const std::string ConsoleActions::CLEAR = "clear"; 
const std::string ConsoleActions::AUTO_SCROLL = "autoscroll"; 
const std::string ConsoleActions::COLLAPSE_LOGS = "collapse_logs"; 
const std::string ConsoleActions::SHOW_ERRORS = "show_errors"; 
const std::string ConsoleActions::SHOW_WARNINGS = "show_warnings"; 
const std::string ConsoleActions::SHOW_INFO = "show_info";
const std::string ConsoleActions::SHOW_SHADER = "show_shader";
const std::string ConsoleActions::SHOW_SYSTEM = "show_system"; 
const std::string ConsoleActions::SHOW_ASSETS = "show_assets"; 
const std::string ConsoleActions::SHOW_OTHER = "show_other"; 

// default toggle settings in the console 
ConsoleToggles ConsoleEngine::toggles = {
    .isAutoScroll = true, 
    .isCollapsedLogs = false, 
    .isShowError = true, 
    .isShowWarning = true, 
    .isShowInfo = true, 
    .isShowShader = true, 
    .isShowSystem = true,
    .isShowAssets = true,
    .isShowUI = true, 
    .isShowOther = true  
};

bool ConsoleEngine::initialize(std::shared_ptr<ConsoleSink> consoleSink)
{
    logSrc = consoleSink;

    // Register Buttons 
    registerButton(ConsoleActions::CLEAR, [](){logSrc->clearLogs();}); 

    // Register Toggles 
    registerToggle(ConsoleActions::AUTO_SCROLL, [](bool state) {
        toggles.isAutoScroll = state; 
    });
    registerToggle(ConsoleActions::COLLAPSE_LOGS, [](bool state) {
        toggles.isCollapsedLogs = state; 
    });

    registerToggle(ConsoleActions::SHOW_ERRORS, [](bool state) {
        toggles.isShowError = state; 
    }); 
    registerToggle(ConsoleActions::SHOW_WARNINGS, [](bool state) {
        toggles.isShowWarning = state; 
    });
    registerToggle(ConsoleActions::SHOW_INFO, [](bool state) {
        toggles.isShowInfo = state; 
    });
    // TODO: implement other filters that filter by the log's source 
    Logger::addLog(LogLevel::INFO, "Console Engine", "This is a test message"); 
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

ConsoleToggles& ConsoleEngine::getToggles() {
    return toggles; 
}