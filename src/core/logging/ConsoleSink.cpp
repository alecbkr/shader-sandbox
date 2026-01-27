#include "ConsoleSink.hpp"

// fetch logs from the static logger to be read by the consoleUI
void ConsoleSink::addLog(const LogEntry& entry) {
    items.push_back(entry); 

    if(items.size() > MAX_HISTORY) {
        items.pop_front(); 
    }
}

void ConsoleSink::clearLogs() {
    while(!items.empty()) {
        items.pop_front(); 
    }
}

const std::deque<LogEntry>& ConsoleSink::getLogs() const{
    return items; 
}