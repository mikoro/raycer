// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Runners/InteractiveSettings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

InteractiveSettings::InteractiveSettings(IniReader& iniReader)
{
	window.width = iniReader.getValue<int>("window", "width");
	window.height = iniReader.getValue<int>("window", "height");
	window.enableFullscreen = iniReader.getValue<bool>("window", "enableFullscreen");
	window.enableVsync = iniReader.getValue<bool>("window", "enableVsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");

	framebuffer.fixedWidth = iniReader.getValue<int>("framebuffer", "fixedWidth");
	framebuffer.fixedHeight = iniReader.getValue<int>("framebuffer", "fixedHeight");
	framebuffer.enableResizing = iniReader.getValue<bool>("framebuffer", "enableResizing");
	framebuffer.resizeScale = iniReader.getValue<double>("framebuffer", "resizeScale");
	framebuffer.enableSmoothing = iniReader.getValue<bool>("framebuffer", "enableSmoothing");

	runner.updateFrequency = iniReader.getValue<double>("runner", "updateFrequency");
	runner.showFps = iniReader.getValue<bool>("runner", "showFps");
	runner.showCameraInfo = iniReader.getValue<bool>("runner", "showCameraInfo");
	runner.infoFont = iniReader.getValue<std::string>("runner", "infoFont");
	runner.infoFontSize = iniReader.getValue<int>("runner", "infoFontSize");
	
	controls.moveSpeed = iniReader.getValue<double>("controls", "moveSpeed");
	controls.mouseSpeed = iniReader.getValue<double>("controls", "mouseSpeed");
	controls.mouseLookWithButton = iniReader.getValue<bool>("controls", "mouseLookWithButton");
}
