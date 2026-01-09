#include "LogSetup.hpp"

LogCtx initLogging() {
    LogCtx ctx; 

    ctx.consoleSink = std::make_shared<ConsoleSink>(); 
    ctx.fileSink = std::make_shared<FileSink>(); 
    ctx.stdoutSink = std::make_shared<StdoutSink>(); 

    Logger::addSink(ctx.consoleSink); 
    Logger::addSink(ctx.fileSink); 
    Logger::addSink(ctx.stdoutSink); 

    return ctx; 
}