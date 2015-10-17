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
	int pid = fork();

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
