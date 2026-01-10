#include "LogSetup.hpp"

LogCtx initLogging() {
    LogCtx ctx; 

    ctx.consoleSink = std::make_shared<ConsoleSink>(); 
    ctx.fileSink = std::make_shared<FileSink>(); 
    ctx.stdoutSink = std::make_shared<StdoutSink>(); 

    Logger::addSink(ctx.consoleSink); 
    Logger::addSink(ctx.fileSink); 
    Logger::addSink(ctx.stdoutSink); 

    // Logger::addLog(LogLevel::CRITICAL, "Test Message", "This is a Critical Error"); 
    // Logger::addLog(LogLevel::ERROR, "Test Message", "This is a Normal Error"); 
    // Logger::addLog(LogLevel::WARNING, "Test Message", "This is a Warning Message"); 
    // Logger::addLog(LogLevel::INFO, "Test Message", "This is an Info Message"); 
    return ctx; 
}