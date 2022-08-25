#include "stdoutSink.hpp"


void StdoutSink::callback(g3::LogMessageMover log) {
	std::cout << log.get().toString(&FormatMsg) << std::flush;
}

std::string StdoutSink::FormatMsg(const g3::LogMessage &msg) {
	std::stringstream ss;
	LogColor color = CustomSink::getColor(msg._level);
	ss << ADD_COLOR(color) 
		<< "[" << msg.timestamp("%H:%M:%S") << "]"
		<< "[" << std::setw(7) << msg.level() << "/"
		<< msg.file() << ":" << msg.line() << "] "
		<< RESET_COLOR;
	return ss.str();
}