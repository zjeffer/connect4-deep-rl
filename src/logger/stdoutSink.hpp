#pragma once

#include "customSink.hpp"

class StdoutSink : public CustomSink
{
  public:
    void callback(g3::LogMessageMover log) override;

    static std::string FormatMsg(g3::LogMessage const& msg);
};