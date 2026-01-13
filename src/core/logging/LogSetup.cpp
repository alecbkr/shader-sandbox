#include "LogSetup.hpp"

// TODO: make this more portable later 
const std::string log_dir = "../src/core/logging/logs";

LogCtx initLogging() {
    LogCtx ctx; 

    ctx.consoleSink = std::make_shared<ConsoleSink>(); 
    ctx.fileSink = std::make_shared<FileSink>(log_dir); 
    ctx.stdoutSink = std::make_shared<StdoutSink>(); 

    Logger::addSink(ctx.consoleSink); 
    Logger::addSink(ctx.fileSink); 
    Logger::addSink(ctx.stdoutSink); 

    // Logger::addLog(LogLevel::ERROR, "Test Message", "This is a Normal Error"); 
    // Logger::addLog(LogLevel::WARNING, "Test Message", "This is a Warning Message"); 
    // Logger::addLog(LogLevel::INFO, "Test Message", "This is an Info Message"); 
    // Logger::addLog(LogLevel::CRITICAL, "Test Message", "This is a Critical Error: ", "Test Critical Error"); 

    return ctx; 
}