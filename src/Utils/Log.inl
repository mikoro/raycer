// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

namespace Raycer
{
	template<typename... Args>
	void Log::logMessage(MessageLevel messageLevel, const std::string& message, const Args&... args)
	{
		handleMessage(messageLevel, tfm::format(message.c_str(), args...));
	}

	template<typename... Args>
	void Log::logDebug(const std::string& message, const Args&... args)
	{
		logMessage(MessageLevel::Debug, message, args...);
	}

	template<typename... Args>
	void Log::logInfo(const std::string& message, const Args&... args)
	{
		logMessage(MessageLevel::Info, message, args...);
	}

	template<typename... Args>
	void Log::logWarning(const std::string& message, const Args&... args)
	{
		logMessage(MessageLevel::Warning, message, args...);
	}

	template<typename... Args>
	void Log::logError(const std::string& message, const Args&... args)
	{
		logMessage(MessageLevel::Error, message, args...);
	}
}
