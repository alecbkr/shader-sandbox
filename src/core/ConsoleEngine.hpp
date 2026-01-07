#pragma once
#include "ConsoleRegister.hpp"
#include <sstream>
#include <string> 
#include <deque>

class ConsoleEngine {
    public:

    ~ConsoleEngine(); 
    // handle copying 
    ConsoleEngine(ConsoleEngine &other) = delete; 
    void operator=(const ConsoleEngine &) = delete; 

    void tokenize(); 

    private:
    std::deque<std::string> tokens; 
}; 