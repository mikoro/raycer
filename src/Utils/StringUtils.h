// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class StringUtils
	{
	public:

		static bool endsWith(const std::string& text, const std::string& end);
	};
}
