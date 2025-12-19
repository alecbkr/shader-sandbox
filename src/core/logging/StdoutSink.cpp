#include "StdoutSink.hpp"

void StdoutSink::addLog(const LogEntry& entry) {
    int idx = std::min((int)entry.level, 4); // clamp the index to avoid out-of-bounds
    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << BG_RED << "CRITICAL:" << RESET_COLORS << " ";          break; 
        case    LogLevel::ERROR:     alert << TEXT_RED << "ERROR:" << RESET_COLORS << " ";           break; 
        case    LogLevel::WARNING:   alert << TEXT_WARNING << "WARNING:" << RESET_COLORS << "";      break; 
        case    LogLevel::INFO:      alert << "INFO: ";                                              break;
        default:                     alert << "ANOMALY: ";                                           break; 
    }    

    std::cout << alert.str() << entry.msg << std::endl; 
}




