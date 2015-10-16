// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/Log.h"

using namespace Raycer;
using namespace std::chrono;

namespace
{
	enum class ConsoleTextColor
	{
		DEFAULT,
		GRAY_ON_BLACK,
		YELLOW_ON_BLACK,
		WHITE_ON_RED
	};

	void setConsoleTextColor(ConsoleTextColor color)
	{
#ifdef _WIN32
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (color)
		{
			case ConsoleTextColor::DEFAULT:
				SetConsoleTextAttribute(consoleHandle, 7 + 0 * 16);
				break;

			case ConsoleTextColor::GRAY_ON_BLACK:
				SetConsoleTextAttribute(consoleHandle, 8 + 0 * 16);
				break;

			case ConsoleTextColor::YELLOW_ON_BLACK:
				SetConsoleTextAttribute(consoleHandle, 14 + 0 * 16);
				break;

			case ConsoleTextColor::WHITE_ON_RED:
				SetConsoleTextAttribute(consoleHandle, 15 + 12 * 16);
				break;

			default: break;
		}
#endif
	}

	void setConsoleTextColorFromMessageLevel(LogMessageLevel messageLevel)
	{
		switch (messageLevel)
		{
			case LogMessageLevel::DEBUG:
				setConsoleTextColor(ConsoleTextColor::GRAY_ON_BLACK);
				break;

			case LogMessageLevel::INFO:
				setConsoleTextColor(ConsoleTextColor::DEFAULT);
				break;

			case LogMessageLevel::WARNING:
				setConsoleTextColor(ConsoleTextColor::YELLOW_ON_BLACK);
				break;

			case LogMessageLevel::ERROR:
				setConsoleTextColor(ConsoleTextColor::WHITE_ON_RED);
				break;

			default: setConsoleTextColor(ConsoleTextColor::DEFAULT); break;
		}
	}
}

Raycer::Log::Log()
{
}

Log::Log(const std::string& logFilePath)
{
	setLogFile(logFilePath);
}

void Log::setLogFile(const std::string& logFilePath)
{
	logFile = std::ofstream(logFilePath);
}

void Log::setMinimumMessageLevel(LogMessageLevel value)
{
	minimumMessageLevel = value;
}

void Log::handleMessage(LogMessageLevel messageLevel, const std::string& message)
{
	if (messageLevel >= minimumMessageLevel)
	{
		std::string formattedMessage = formatMessage(messageLevel, message);
		outputMessage(messageLevel, formattedMessage);
	}
}

std::string Log::formatMessage(LogMessageLevel messageLevel, const std::string& message)
{
	std::string messageLevelName;

	switch (messageLevel)
	{
		case LogMessageLevel::DEBUG:
			messageLevelName = "Debug";
			break;

		case LogMessageLevel::INFO:
			messageLevelName = "Info";
			break;

		case LogMessageLevel::WARNING:
			messageLevelName = "Warning";
			break;

		case LogMessageLevel::ERROR:
			messageLevelName = "Error";
			break;

		default: messageLevelName = "Unknown"; break;
	}

	auto now = std::chrono::system_clock::now();
	auto epoch = now.time_since_epoch();
	time_t tt = system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
	milliseconds = milliseconds - seconds * 1000;

	return tfm::format("%02d:%02d:%02d.%03d [%s] %s", local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec, milliseconds, messageLevelName, message);
}

void Log::outputMessage(LogMessageLevel messageLevel, const std::string& message)
{
	std::lock_guard<std::mutex> lock(outputMutex);

	setConsoleTextColorFromMessageLevel(messageLevel);
	std::cout << message << std::endl;
	setConsoleTextColor(ConsoleTextColor::DEFAULT);

	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.flush();
	}
}

void Log::logMessage(LogMessageLevel messageLevel, const std::string& message)
{
	handleMessage(messageLevel, message);
}

void Log::logDebug(const std::string& message)
{
	logMessage(LogMessageLevel::DEBUG, message);
}

void Log::logInfo(const std::string& message)
{
	logMessage(LogMessageLevel::INFO, message);
}

void Log::logWarning(const std::string& message)
{
	logMessage(LogMessageLevel::WARNING, message);
}

void Log::logError(const std::string& message)
{
	logMessage(LogMessageLevel::ERROR, message);
}

void Log::logException(const std::exception_ptr& exception)
{
	try
	{
		std::rethrow_exception(exception);
	}
	catch (const std::exception& ex)
	{
		logError("Exception: %s: %s", typeid(ex).name(), ex.what());
	}
	catch (const std::string& s)
	{
		logError("Exception: %s", s);
	}
	catch (...)
	{
		logError("Unknown exception!");
	}
}
