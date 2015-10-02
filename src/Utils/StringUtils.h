// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class StringUtils
	{
	public:

		static bool endsWith(const std::string& input, const std::string& end);
		static bool readUntilSpace(const std::string& input, size_t& startIndex, std::string& result);
		static double parseDouble(const std::string& input);
	};
}
