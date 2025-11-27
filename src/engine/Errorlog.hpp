#ifndef ERRORLOG_HPP
#define ERRORLOG_HPP

#define ERRLOG Errorlog::getInstance() 

#include <string>

typedef enum ErrLevel {
    info,
    warning,
    error,
    critical
} ErrLevel;


class Errorlog {
    public:
        static Errorlog &getInstance();
        void logEntry(ErrLevel level, const char *src, const char *msg);
        void logEntry(ErrLevel level, const char *src, const char *msg, const char *additional);
        void printClear();
        void setAbortWhen(ErrLevel level);

    private:
        typedef struct errEntry {
            ErrLevel level;
            std::string src;
            std::string msg;
        } ErrEntry;

        ErrEntry errlog[256];
        int idx;
        ErrLevel abortWhen;
        Errorlog();
        void print(ErrEntry &entry);
        void discard(ErrEntry &entry);
};


#endif