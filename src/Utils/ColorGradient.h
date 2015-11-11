// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Math/Color.h"

namespace Raycer
{
	struct ColorGradientSegment
	{
		Color startColor;
		Color endColor;
		uint64_t startIndex = 0;
		uint64_t endIndex = 0;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(startColor),
				CEREAL_NVP(endColor),
				CEREAL_NVP(startIndex),
				CEREAL_NVP(endIndex));
		}
	};

	class ColorGradient
	{
	public:

		void addSegment(const Color& start, const Color& end, uint64_t length);
		Color getColor(double alpha) const;

	private:

		uint64_t totalLength = 0;
		std::vector<ColorGradientSegment> segments;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(totalLength),
				CEREAL_NVP(segments));
		}
	};
}
