#include "ConsoleEngine.hpp"
#include "logging/Logger.hpp"
#include <sstream> 


bool isWhiteSpace(const std::string& str) {
    if (str.empty()) return false; 

    for (unsigned char c: str) {
        if (!std::isspace(c)) {
            return false;
        }
    }
    return true;
}

bool ConsoleEngine::initialize(Logger* _loggerPtr) {
    if (initialized) {
        _loggerPtr->addLog(LogLevel::WARNING, "Console UI Initialization", "Console UI was already initialized.");
        return false;
    }

    logSrc = _loggerPtr->getConsoleSinkPtr();
    loggerPtr = _loggerPtr; 

    // Register Buttons
    registerButton(ConsoleActions::CLEAR, [this](){
        if (logSrc) logSrc->clearLogs();
    });

    registerButton(ConsoleActions::OPEN_LOG_HISTORY, [this](){
        if (loggerPtr) openLogFile(loggerPtr->getLogPath().string());
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

bool ConsoleEngine::isLogFiltered(const LogEntry& log) const{

    // Message type
    if (log.level == LogLevel::LOG_ERROR && !toggles.isShowError) return true;
    if (log.level == LogLevel::WARNING && !toggles.isShowWarning) return true;
    if (log.level == LogLevel::INFO && !toggles.isShowInfo) return true; 

    // source types 
    if (log.category == LogCategory::UI && !toggles.isShowUI) return true; 
    if (log.category == LogCategory::ASSETS && !toggles.isShowAssets) return true; 
    if (log.category == LogCategory::SHADER && !toggles.isShowShader) return true; 
    if (log.category == LogCategory::OTHER && !toggles.isShowOther) return true;  
    if (log.category == LogCategory::SYSTEM && !toggles.isShowSystem) return true;
    
    // auto filter empty messages 
    if (isWhiteSpace(log.msg) || log.msg == "\n" || log.msg == "\t") return true; 

    return false; 
}

void ConsoleEngine::openLogFile(const std::string logPath) {
    if(logPath.empty()) {
        loggerPtr->addLog(LogLevel::WARNING, "ConsoleUI", "Cannot open empty log path."); 
        return; 
    }

    std::filesystem::path p(logPath); 
    std::error_code ec; 

    if (!std::filesystem::exists(p, ec)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ConsoleUI", "Log directory not found at: " + logPath);
        return; 
    }

    std::filesystem::path absPath = std::filesystem::absolute(p, ec); 
    if (ec) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ConsoleUI", "Failed to obtain absolute path.");
        return;  
    }

    absPath.make_preferred(); 
    std::string finalPath = absPath.string(); 
    std::string folderPath; 

    #if defined(_WIN32) || defined(__CYGWIN__) 
        folderPath = "explorer.exe /e, \"" + finalPath +"\"";

    // TODO: Test unix os's to see if they work 
    #elif defined(__APPLE__)
        folderPath = "open \"" + finalPath + "\""; 

    #elif defined(__linux__)
        folderPath = "xdg-open \"" + finalPath + "\""; 
    #else 
        loggerPtr(LogLevel::Error, "Console", "Macro not defined or unsupported os"); 
        return; 
    #endif

    std::system(folderPath.c_str()); 
}

std::string ConsoleEngine::getFilteredLogText() const {
    if (!logSrc) return "";

    std::stringstream ss;
    const auto& currentLogs = logSrc->getLogs();

    for (const auto& log : currentLogs) {
        if (!isLogFiltered(log)) {
            std::string prefix;
            switch (log.level) {
                case LogLevel::CRITICAL: prefix = "[CRITICAL"; break;
                case LogLevel::LOG_ERROR: prefix = "[ERROR"; break;
                case LogLevel::WARNING: prefix = "[WARNING"; break;
                case LogLevel::INFO: prefix = "[INFO"; break;
            }

                if(!log.src.empty()) {
                    prefix += ": " + log.src;
                }
                prefix += "] ";

                ss << prefix << log.msg;
                if (!log.additional.empty()) {
                    ss << " " << log.additional;
                }
                ss << "\n";
        }
    }

    return ss.str();
}