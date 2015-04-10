// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Utils/Settings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

void Settings::load(const std::string& fileName)
{
	IniReader iniReader;
	iniReader.readFile(fileName);

	scene.fileName = iniReader.getValue<std::string>("scene", "fileName");

	image.width = iniReader.getValue<int>("image", "width");
	image.height = iniReader.getValue<int>("image", "height");
	image.fileName = iniReader.getValue<std::string>("image", "fileName");
	image.autoView = iniReader.getValue<bool>("image", "autoView");

	general.interactive = iniReader.getValue<bool>("general", "interactive");
	general.useOpenCL = iniReader.getValue<bool>("general", "useOpenCL");
	general.openCLPlatform = iniReader.getValue<int>("general", "openCLPlatform");
	general.openCLDevice = iniReader.getValue<int>("general", "openCLDevice");

	window.width = iniReader.getValue<int>("window", "width");
	window.height = iniReader.getValue<int>("window", "height");
	window.fullscreen = iniReader.getValue<bool>("window", "fullscreen");
	window.vsync = iniReader.getValue<bool>("window", "vsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");

	framebuffer.scale = iniReader.getValue<double>("framebuffer", "scale");
	framebuffer.smoothing = iniReader.getValue<bool>("framebuffer", "smoothing");

	interactive.showFps = iniReader.getValue<bool>("interactive", "showFps");
	interactive.showCameraInfo = iniReader.getValue<bool>("interactive", "showCameraInfo");
	interactive.infoFont = iniReader.getValue<std::string>("interactive", "infoFont");
	interactive.infoFontSize = iniReader.getValue<int>("interactive", "infoFontSize");
	
	controls.moveSpeed = iniReader.getValue<double>("controls", "moveSpeed");
	controls.mouseSpeed = iniReader.getValue<double>("controls", "mouseSpeed");
	controls.freeLook = iniReader.getValue<bool>("controls", "freeLook");
}
