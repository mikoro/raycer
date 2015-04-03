// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "tinyformat/tinyformat.h"

namespace Raycer
{
	class NamedLog;

	class BaseLog
	{
	public:

		friend class NamedLog;

		enum class MessageLevel { Debug = 0, Info = 1, Warning = 2, Error = 3 };

		BaseLog(const std::string& fileName);
		~BaseLog();

		std::unique_ptr<NamedLog> getNamedLog(const std::string& name);

		void setMinimumMessageLevel(MessageLevel value);
		void setOutputToConsole(bool value);

	private:

		void handleMessage(MessageLevel messageLevel, const std::string& sourceName, const std::string& message);
		std::string formatMessage(MessageLevel messageLevel, const std::string& sourceName, const std::string& message);
		void outputMessage(const std::string& message);

		std::ofstream file;
		std::mutex outputMutex;
		MessageLevel minimumMessageLevel = MessageLevel::Debug;
		bool outputToConsole = true;
	};

	class NamedLog
	{
	public:

		NamedLog(BaseLog& baseLog, const std::string& name);

		BaseLog& getBaseLog() const;

		void logMessage(BaseLog::MessageLevel messageLevel, const std::string& message);
		void logDebug(const std::string& message);
		void logInfo(const std::string& message);
		void logWarning(const std::string& message);
		void logError(const std::string& message);
		void logException(const std::exception_ptr& exception);

		template<typename... Args>
		void logMessage(BaseLog::MessageLevel messageLevel, const std::string& message, const Args&... args);

		template<typename... Args>
		void logDebug(const std::string& message, const Args&... args);

		template<typename... Args>
		void logInfo(const std::string& message, const Args&... args);

		template<typename... Args>
		void logWarning(const std::string& message, const Args&... args);

		template<typename... Args>
		void logError(const std::string& message, const Args&... args);

	private:

		NamedLog(const NamedLog& n);
		void operator=(const NamedLog& n); // suppress a warning

		BaseLog& baseLog;
		std::string name;
	};
}

template<typename... Args>
void Raycer::NamedLog::logMessage(BaseLog::MessageLevel messageLevel, const std::string& message, const Args&... args)
{
	baseLog.handleMessage(messageLevel, name, tfm::format(message.c_str(), args...));
}

template<typename... Args>
void Raycer::NamedLog::logDebug(const std::string& message, const Args&... args)
{
	logMessage(BaseLog::MessageLevel::Debug, message, args...);
}

template<typename... Args>
void Raycer::NamedLog::logInfo(const std::string& message, const Args&... args)
{
	logMessage(BaseLog::MessageLevel::Info, message, args...);
}

template<typename... Args>
void Raycer::NamedLog::logWarning(const std::string& message, const Args&... args)
{
	logMessage(BaseLog::MessageLevel::Warning, message, args...);
}

template<typename... Args>
void Raycer::NamedLog::logError(const std::string& message, const Args&... args)
{
	logMessage(BaseLog::MessageLevel::Error, message, args...);
}
