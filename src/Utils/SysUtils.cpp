// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/SysUtils.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

void SysUtils::openFileExternally(const std::string& filePath)
{
	Log& log = App::getLog();
	log.logInfo("Opening file in an external viewer (%s)", filePath);

#ifdef _WIN32
	ShellExecuteA(nullptr, "open", filePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
	int32_t pid = fork();

	if (pid == 0)
	{
#ifdef __linux
		char* arg[] = { (char*)"xdg-open", (char*)filePath.c_str(), (char*)0 };
#elif __APPLE__
		char* arg[] = { (char*)"open", (char*)filePath.c_str(), (char*)0 };
#endif
		if (execvp(arg[0], arg) == -1)
			log.logWarning("Could not open file externally (%d)", errno);
	}
#endif
}

void SysUtils::setConsoleTextColor(ConsoleTextColor color)
{
	(void)color;

#ifdef _WIN32
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	if (consoleHandle == nullptr)
		return;

	switch (color)
	{
		case ConsoleTextColor::DEFAULT:
			SetConsoleTextAttribute(consoleHandle, 7 + 0 * 16);
			break;

		case ConsoleTextColor::GRAY_ON_BLACK:
			SetConsoleTextAttribute(consoleHandle, 8 + 0 * 16);
			break;

		case ConsoleTextColor::WHITE_ON_BLACK:
			SetConsoleTextAttribute(consoleHandle, 15 + 0 * 16);
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
