#pragma once

#include <g3log/logmessage.hpp>
#include <iomanip>
#include <iostream>
#include <string>

/**
 * @brief Enum for text color of log messages
 *
 */
enum LogColor {
	RESET,
	WHITE = 97,
	BLUE = 94,
	GREEN = 32,
	YELLOW = 33,
	RED = 31
};

// macro to add color to text
#define ADD_COLOR(color) "\x1b[" << int(color) << "m"
// reset color to default
#define RESET_COLOR "\x1b[" << int(LogColor::RESET) << "m"

/**
 * @brief Interface for a custom G3LOG sink.
 *
 */
class CustomSink {
  public:
	virtual ~CustomSink() = default;

	/**
	 * @brief Callback function for the g3log addSink method.
	 *
	 * @param log: contains the message before formatting.
	 */
	virtual void callback(g3::LogMessageMover log) = 0;

	static LogColor getColor(const LEVELS level) {
		switch (level.value) {
		case g3::kDebugValue:
			return LogColor::BLUE;
		case g3::kInfoValue:
			return LogColor::GREEN;
		case g3::kWarningValue:
			return LogColor::YELLOW;
		case g3::kFatalValue:
			return LogColor::RED;
		default:
			return g3::internal::wasFatal(level) ? LogColor::RED
												 : LogColor::WHITE;
		}
	}
};
