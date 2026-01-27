#pragma once
#include <memory>
#include "ConsoleSink.hpp"
#include "FileSink.hpp"
#include "StdoutSink.hpp"
#include "Logger.hpp"

// holds pointers for the logging system to integrate with other systems if need be 
struct LogCtx {
    std::shared_ptr<ConsoleSink> consoleSink; 
    std::shared_ptr<FileSink> fileSink; 
    std::shared_ptr<StdoutSink> stdoutSink; 
}; 

LogCtx initLogging(); 