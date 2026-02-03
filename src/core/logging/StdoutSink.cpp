#include "StdoutSink.hpp"

void StdoutSink::addLog(const LogEntry& entry) {
    std::stringstream alert; 

    switch (entry.level) {
        case    LogLevel::CRITICAL:  alert << BG_RED << "[CRITICAL: " <<  " ";         break; 
        case    LogLevel::LOG_ERROR:     alert << TEXT_RED << "[ERROR: " <<  " ";          break; 
        case    LogLevel::WARNING:   alert << TEXT_WARNING << "[WARNING: " << "";      break; 
        case    LogLevel::INFO:      alert << "[INFO: ";                               break;
        default:                     alert << "[ANOMALY: ";                            break; 
    }    

    std::cout << alert.str() << entry.src << "]" << RESET_COLORS << " " << entry.msg << " " << entry.additional << std::endl; 
}




