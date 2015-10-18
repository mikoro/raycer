// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	enum class ConsoleTextColor
	{
		DEFAULT,
		GRAY_ON_BLACK,
		WHITE_ON_BLACK,
		YELLOW_ON_BLACK,
		WHITE_ON_RED
	};

	class SysUtils
	{
	public:

		static void openFileExternally(const std::string& filePath);
		static void setConsoleTextColor(ConsoleTextColor color);
	};
}
