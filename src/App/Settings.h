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
			float fov;
		} window;

		struct Framebuffer
		{
			int fixedWidth;
			int fixedHeight;
			bool enableResizing;
			float resizeScale;
			bool enableSmoothing;
		} framebuffer;

		struct Log
		{
			int minimumMessageLevel;
			bool outputToConsole;
		} logger;

		struct App
		{
			float updateFrequency;
			bool showFps;
			std::string fpsFont;
			int fpsFontSize;
		} app;

		struct Controls
		{
			float moveSpeed;
			float mouseSpeed;
			bool mouseLookWithButton;
		} controls;
	};
}
