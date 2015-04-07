// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class IniReader;

	struct Settings
	{
		Settings(IniReader& iniReader);

		struct Window
		{
			int width;
			int height;
			bool enableFullscreen;
			bool enableVsync;
			bool hideCursor;
		} window;

		struct Framebuffer
		{
			int fixedWidth;
			int fixedHeight;
			bool enableResizing;
			double resizeScale;
			bool enableSmoothing;
		} framebuffer;

		struct Log
		{
			int minimumMessageLevel;
			bool outputToConsole;
		} logger;

		struct App
		{
			double updateFrequency;
			bool showFps;
			bool showCameraInfo;
			std::string infoFont;
			int infoFontSize;
		} app;

		struct Controls
		{
			double moveSpeed;
			double mouseSpeed;
			bool mouseLookWithButton;
		} controls;
	};
}
