// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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
		} app;
	};
}
