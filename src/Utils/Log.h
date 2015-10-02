// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <exception>
#include <fstream>
#include <mutex>

namespace Raycer
{
	enum class MessageLevel { Debug = 0, Info = 1, Warning = 2, Error = 3 };

	class Log
	{
	public:

		Log(const std::string& fileName);
		~Log();

		void setMinimumMessageLevel(MessageLevel value);

		void logMessage(MessageLevel messageLevel, const std::string& message);
		void logDebug(const std::string& message);
		void logInfo(const std::string& message);
		void logWarning(const std::string& message);
		void logError(const std::string& message);
		void logException(const std::exception_ptr& exception);

		template<typename... Args>
		void logMessage(MessageLevel messageLevel, const std::string& message, const Args&... args);

		template<typename... Args>
		void logDebug(const std::string& message, const Args&... args);

		template<typename... Args>
		void logInfo(const std::string& message, const Args&... args);

		template<typename... Args>
		void logWarning(const std::string& message, const Args&... args);

		template<typename... Args>
		void logError(const std::string& message, const Args&... args);

	private:

		Log();
		Log(const Log& log);
		Log& operator=(const Log& log);

		void handleMessage(MessageLevel messageLevel, const std::string& message);
		std::string formatMessage(MessageLevel messageLevel, const std::string& message);
		void outputMessage(const std::string& message);

		std::ofstream file;
		std::mutex outputMutex;
		MessageLevel minimumMessageLevel = MessageLevel::Debug;
	};
}

#include "Utils/Log.inl"
