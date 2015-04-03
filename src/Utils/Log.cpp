// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <chrono>
#include <ctime>
#include <iomanip>

#include "Utils/Log.h"

using namespace Raycer;

BaseLog::BaseLog(const std::string& fileName)
{
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file.open(fileName, std::ios_base::out);
}

BaseLog::~BaseLog()
{
	file.close();
}

std::unique_ptr<NamedLog> BaseLog::getNamedLog(const std::string& name)
{
	return std::unique_ptr<NamedLog>(new NamedLog(*this, name));
}

void BaseLog::setMinimumMessageLevel(MessageLevel value)
{
	minimumMessageLevel = value;
}

void BaseLog::setOutputToConsole(bool value)
{
	outputToConsole = value;
}

void BaseLog::handleMessage(MessageLevel messageLevel, const std::string& sourceName, const std::string& message)
{
	if (messageLevel >= minimumMessageLevel)
	{
		std::string formattedMessage = formatMessage(messageLevel, sourceName, message);
		outputMessage(formattedMessage);
	}
}

std::string BaseLog::formatMessage(MessageLevel messageLevel, const std::string& sourceName, const std::string& message)
{
	std::string messageLevelName = "Unknown";

	switch (messageLevel)
	{
		case MessageLevel::Debug: messageLevelName = "Debug"; break;
		case MessageLevel::Info: messageLevelName = "Info"; break;
		case MessageLevel::Warning: messageLevelName = "Warning"; break;
		case MessageLevel::Error: messageLevelName = "Error"; break;
	}

	std::stringstream timeStringBuffer;
	std::time_t t = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &t);
	timeStringBuffer << std::put_time(&tm, "%H:%M:%S");

	auto epochDuration = std::chrono::system_clock::now().time_since_epoch();
	auto epochDurationS = std::chrono::duration_cast<std::chrono::seconds>(epochDuration).count();
	auto epochDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(epochDuration).count();
	auto milliSeconds = epochDurationMs - epochDurationS * 1000;

	return tfm::format("%s.%03d [%s] %s - %s", timeStringBuffer.str(), milliSeconds, messageLevelName, sourceName, message);
}

void BaseLog::outputMessage(const std::string& message)
{
	std::lock_guard<std::mutex> lock(outputMutex);

	if (outputToConsole)
		std::cout << message << std::endl;

	if (file.is_open())
	{
		file << message << std::endl;
		file.flush();
	}
}

NamedLog::NamedLog(BaseLog& baseLog_, const std::string& name_) : baseLog(baseLog_), name(name_)
{
}

BaseLog& NamedLog::getBaseLog() const
{
	return baseLog;
}

void NamedLog::logMessage(BaseLog::MessageLevel messageLevel, const std::string& message)
{
	baseLog.handleMessage(messageLevel, name, message);
}

void NamedLog::logDebug(const std::string& message)
{
	logMessage(BaseLog::MessageLevel::Debug, message);
}

void NamedLog::logInfo(const std::string& message)
{
	logMessage(BaseLog::MessageLevel::Info, message);
}

void NamedLog::logWarning(const std::string& message)
{
	logMessage(BaseLog::MessageLevel::Warning, message);
}

void NamedLog::logError(const std::string& message)
{
	logMessage(BaseLog::MessageLevel::Error, message);
}

void NamedLog::logException(const std::exception_ptr& exception)
{
	try
	{
		std::rethrow_exception(exception);
	}
	catch (const std::exception& ex)
	{
		logError("Exception (%s): %s", typeid(ex).name(), ex.what());
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
