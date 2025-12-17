#include "Errorlog.hpp"

#include <iostream>


Errorlog& Errorlog::getInstance() {
    static Errorlog instance;
    return instance;
}


void Errorlog::logEntry(ErrLevel level, const char *src, const char *msg) {
    idx++;
    if (idx > 255) {
        std::cerr << "ERRLOG FULL, ENTRY LOST" << std::endl;
        idx = 255;
        return;
    }
    
    ErrEntry &currEntry = errlog[idx];
    currEntry.level = level;
    currEntry.src = std::string(src);
    currEntry.msg = std::string(msg);

    //Emergency exit in case EL_CRITICAL EL_ERROR to avoid more errors
    if (currEntry.level >= abortWhen) {
        print(currEntry);
        exit(1);
    }
}


void Errorlog::logEntry(ErrLevel level, const char *srcIn, const char *msgIn, const char *add) {
    idx++;
    if (idx > 255) {
        std::cerr << "ERRLOG FULL, ENTRY LOST" << std::endl;
        idx = 255;
        return;
    }
    ErrEntry &currEntry = errlog[idx];
    currEntry.level = level;
    currEntry.src = std::string(srcIn);
    currEntry.msg = msgIn;
    currEntry.msg.append(" \"");
    currEntry.msg.append(add);
    currEntry.msg.append("\"");

    //Emergency exit in case EL_CRITICAL EL_ERROR to avoid more errors
    if (currEntry.level >= abortWhen) {
        print(currEntry);
        exit(1);
    }
}


void Errorlog::logEntry(ErrLevel level, const char *src, const char *msg, int value) {
    idx++;
    if (idx > 255) {
        std::cerr << "ERRLOG FULL, ENTRY LOST" << std::endl;
        idx = 255;
        return;
    }
    
    ErrEntry &currEntry = errlog[idx];
    currEntry.level = level;
    currEntry.src = std::string(src);
    currEntry.msg = std::string(msg);
    currEntry.msg.append(std::to_string(value));

    //Emergency exit in case EL_CRITICAL EL_ERROR to avoid more errors
    if (currEntry.level >= abortWhen) {
        print(currEntry);
        exit(1);
    }
}


void Errorlog::printClear() {
    if (idx == -1) return;

    for (int currIdx = 0; currIdx <= idx; currIdx++) {
        ErrEntry &currEntry = errlog[currIdx];
        print(currEntry);
        discard(currEntry);
    }
    idx = -1;
}


void Errorlog::announce(const char *msg) {
    std::cerr << "[ANNOUNCEMENT] " << msg << std::endl;
}


void Errorlog::announce(const char *msg, int additional) {
    std::cerr << "[ANNOUNCEMENT] " << msg << ": " << additional << std::endl;
}


void Errorlog::setAbortWhen(ErrLevel level) {
    abortWhen = level;
}



Errorlog::Errorlog() {
    idx = -1;
}


void Errorlog::print(ErrEntry &entry) {
    const char *alert;
    switch (entry.level) {
        case EL_INFO:     alert = "INFO: ";     break;
        case EL_WARNING:  alert = "WARNING: ";  break;
        case EL_ERROR:    alert = "ERROR: ";    break;
        case EL_CRITICAL: alert = "CRITICAL: "; break;
        default:       alert = "anomaly: ";   break;
    }
    std::cerr << "[" << alert << entry.src << "] " << entry.msg << std::endl;
}


void Errorlog::discard(ErrEntry &entry) {
    entry.src = "";
    entry.msg = "";
    entry.level = EL_INFO;
}