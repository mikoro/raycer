// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/Log.h"

using namespace Raycer;
using namespace std::chrono;

Log::Log()
{
}

Log::Log(const std::string& fileName)
{
	file.open(fileName, std::ios_base::out);
}

Log::~Log()
{
	file.close();
}

void Log::setMinimumMessageLevel(MessageLevel value)
{
	minimumMessageLevel = value;
}

void Log::handleMessage(MessageLevel messageLevel, const std::string& message)
{
	if (messageLevel >= minimumMessageLevel)
	{
		std::string formattedMessage = formatMessage(messageLevel, message);
		outputMessage(formattedMessage);
	}
}

std::string Log::formatMessage(MessageLevel messageLevel, const std::string& message)
{
	std::string messageLevelName = "Unknown";

	switch (messageLevel)
	{
		case MessageLevel::Debug: messageLevelName = "Debug"; break;
		case MessageLevel::Info: messageLevelName = "Info"; break;
		case MessageLevel::Warning: messageLevelName = "Warning"; break;
		case MessageLevel::Error: messageLevelName = "Error"; break;
	}

	auto now = std::chrono::system_clock::now();
	auto epoch = now.time_since_epoch();
	time_t tt = system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);
	int seconds = (int)std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
	milliseconds = milliseconds - seconds * 1000;

	return tfm::format("%02d:%02d:%02d.%03d [%s] %s", local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec, milliseconds, messageLevelName, message);
}

void Log::outputMessage(const std::string& message)
{
	std::lock_guard<std::mutex> lock(outputMutex);
	std::cout << message << std::endl;

	if (file.is_open())
	{
		file << message << std::endl;
		file.flush();
	}
}

void Log::logMessage(MessageLevel messageLevel, const std::string& message)
{
	handleMessage(messageLevel, message);
}

void Log::logDebug(const std::string& message)
{
	logMessage(MessageLevel::Debug, message);
}

void Log::logInfo(const std::string& message)
{
	logMessage(MessageLevel::Info, message);
}

void Log::logWarning(const std::string& message)
{
	logMessage(MessageLevel::Warning, message);
}

void Log::logError(const std::string& message)
{
	logMessage(MessageLevel::Error, message);
}

void Log::logException(const std::exception_ptr& exception)
{
	try
	{
		std::rethrow_exception(exception);
	}
	catch (const std::exception& ex)
	{
		logError("%s: %s", typeid(ex).name(), ex.what());
	}
	catch (const std::string& s)
	{
		logError(s);
	}
	catch (...)
	{
		logError("Unknown error!");
	}
}
