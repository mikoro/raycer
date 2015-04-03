// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "App/Settings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

Settings::Settings(IniReader& iniReader)
{
	window.width = iniReader.getValue<int>("window", "width");
	window.height = iniReader.getValue<int>("window", "height");
	window.enableFullscreen = iniReader.getValue<bool>("window", "enableFullscreen");
	window.enableVsync = iniReader.getValue<bool>("window", "enableVsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");

	framebuffer.fixedWidth = iniReader.getValue<int>("framebuffer", "fixedWidth");
	framebuffer.fixedHeight = iniReader.getValue<int>("framebuffer", "fixedHeight");
	framebuffer.enableResizing = iniReader.getValue<bool>("framebuffer", "enableResizing");
	framebuffer.resizeScale = iniReader.getValue<float>("framebuffer", "resizeScale");
	framebuffer.enableSmoothing = iniReader.getValue<bool>("framebuffer", "enableSmoothing");

	logger.minimumMessageLevel = iniReader.getValue<int>("logger", "minimumMessageLevel");
	logger.outputToConsole = iniReader.getValue<bool>("logger", "outputToConsole");

	app.updateFrequency = iniReader.getValue<float>("app", "updateFrequency");
	app.showFps = iniReader.getValue<bool>("app", "showFps");
}
