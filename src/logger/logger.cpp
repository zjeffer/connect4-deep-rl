#include "logger.hpp"

#include <memory>

#include "stdoutSink.hpp"
#include <g3log/loglevels.hpp>

Logger::Logger()
  : logWorker(g3::LogWorker::createLogWorker())
  , stdoutSink(std::make_unique<StdoutSink>())
{
    logWorker->addSink(std::move(stdoutSink), &StdoutSink::callback);

    g3::initializeLogging(logWorker.get());
}

Logger::~Logger()
{
    // TODO: this segfaults for some reason
    // it exits fine if not included
    // this->destroy();
}

void Logger::destroy()
{
    std::cout << "Destroying logger" << std::endl;
    logWorker->removeAllSinks();
    logWorker.reset();
}