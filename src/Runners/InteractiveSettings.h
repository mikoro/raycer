// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class IniReader;

	struct InteractiveSettings
	{
		InteractiveSettings(IniReader& iniReader);

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

		struct Runner
		{
			double updateFrequency;
			bool showFps;
			bool showCameraInfo;
			std::string infoFont;
			int infoFontSize;
		} runner;

		struct Controls
		{
			double moveSpeed;
			double mouseSpeed;
			bool mouseLookWithButton;
		} controls;
	};
}
