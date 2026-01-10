#include "StdoutSink.hpp"

void StdoutSink::addLog(const LogEntry& entry) {
    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << BG_RED << "CRITICAL: " << RESET_COLORS << " ";          break; 
        case    LogLevel::ERROR:     alert << TEXT_RED << "ERROR: " << RESET_COLORS << " ";           break; 
        case    LogLevel::WARNING:   alert << TEXT_WARNING << "WARNING: " << RESET_COLORS << "";      break; 
        case    LogLevel::INFO:      alert << "INFO: ";                                               break;
        default:                     alert << "ANOMALY: ";                                            break; 
    }    

    std::cout << '[' << alert.str() << entry.src << "] " << entry.msg << entry.additional << std::endl; 
}




