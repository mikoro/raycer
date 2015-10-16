// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

namespace Raycer
{
	template <typename... Args>
	void Log::logMessage(LogMessageLevel messageLevel, const std::string& message, const Args&... args)
	{
		handleMessage(messageLevel, tfm::format(message.c_str(), args...));
	}

	template <typename... Args>
	void Log::logDebug(const std::string& message, const Args&... args)
	{
		logMessage(LogMessageLevel::DEBUG, message, args...);
	}

	template <typename... Args>
	void Log::logInfo(const std::string& message, const Args&... args)
	{
		logMessage(LogMessageLevel::INFO, message, args...);
	}

	template <typename... Args>
	void Log::logWarning(const std::string& message, const Args&... args)
	{
		logMessage(LogMessageLevel::WARNING, message, args...);
	}

	template <typename... Args>
	void Log::logError(const std::string& message, const Args&... args)
	{
		logMessage(LogMessageLevel::ERROR, message, args...);
	}
}
