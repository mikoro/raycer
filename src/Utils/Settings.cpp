// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Utils/Settings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

void Settings::load(const std::string& fileName)
{
	IniReader iniReader;
	iniReader.readFile(fileName);

	general.interactive = iniReader.getValue<bool>("general", "interactive");

	openCL.enabled = iniReader.getValue<bool>("openCL", "enabled");
	openCL.platformId = iniReader.getValue<int>("openCL", "platformId");
	openCL.deviceType = iniReader.getValue<int>("openCL", "deviceType");
	openCL.deviceId = iniReader.getValue<int>("openCL", "deviceId");
	openCL.options = iniReader.getValue("openCL", "options");

	scene.fileName = iniReader.getValue("scene", "fileName");

	image.width = iniReader.getValue<size_t>("image", "width");
	image.height = iniReader.getValue<size_t>("image", "height");
	image.fileName = iniReader.getValue("image", "fileName");
	image.autoView = iniReader.getValue<bool>("image", "autoView");

	window.width = iniReader.getValue<size_t>("window", "width");
	window.height = iniReader.getValue<size_t>("window", "height");
	window.fullscreen = iniReader.getValue<bool>("window", "fullscreen");
	window.vsync = iniReader.getValue<bool>("window", "vsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");
	window.showFps = iniReader.getValue<bool>("window", "showFps");
	window.showCameraInfo = iniReader.getValue<bool>("window", "showCameraInfo");
	window.defaultFont = iniReader.getValue("window", "defaultFont");
	window.defaultFontSize = iniReader.getValue<int>("window", "defaultFontSize");

	framebuffer.scale = iniReader.getValue<double>("framebuffer", "scale");
	framebuffer.smoothing = iniReader.getValue<bool>("framebuffer", "smoothing");
	framebuffer.vertexShader = iniReader.getValue("framebuffer", "vertexShader");
	framebuffer.fragmentShader = iniReader.getValue("framebuffer", "fragmentShader");

	controls.moveSpeed = iniReader.getValue<double>("controls", "moveSpeed");
	controls.mouseSpeed = iniReader.getValue<double>("controls", "mouseSpeed");
	controls.freeLook = iniReader.getValue<bool>("controls", "freeLook");
}
