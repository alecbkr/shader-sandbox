// DESCRIPTION
/*
Errorlog is a single instance of collected messages during some interval during runtime.
If a CRITICAL error is recorded, the program will print ONLY that message and exit automatically.
This is to prevent additional errors being logged due to the critical failure.

ERRLOG       - returns the instance for class function calls

logEntry()   - Inserts message in log. if full, entry will be lost. 

printClear() - prints all logged messages to cerr and wipes the log.
               Call after initialization and once every frame.

announce()   - quick print to cerr, prints immediately

*/

#ifndef ERRORLOG_HPP
#define ERRORLOG_HPP

#include <string>

#define ERRLOG Errorlog::getInstance() 

enum ErrLevel {
    EL_INFO,
    EL_WARNING,
    EL_ERROR,
    EL_CRITICAL
};


class Errorlog {
    public:
        static Errorlog &getInstance();
        void logEntry(ErrLevel level, const char *src, const char *msg);
        void logEntry(ErrLevel level, const char *src, const char *msg, const char *additional);
        void logEntry(ErrLevel level, const char *src, const char *msg, int additional);
        void printClear();
        void announce(const char *msg);
        void setAbortWhen(ErrLevel level);

    private:
        struct ErrEntry {
            ErrLevel level;
            std::string src;
            std::string msg;
        };

        ErrEntry errlog[256];
        int idx;
        ErrLevel abortWhen = EL_CRITICAL; // set what warnings halt the program
        Errorlog();
        void print(ErrEntry &entry);
        void discard(ErrEntry &entry);
};


#endif