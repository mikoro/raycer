// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Utils/StringUtils.h"

using namespace Raycer;

bool StringUtils::endsWith(const std::string& text, const std::string& end)
{
	return text.rfind(end) == (text.size() - end.size());
}
