// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class MovingAverage
	{
	public:

		MovingAverage(double alpha = 1.0, double averageValue = 0.0);

		void setAlpha(double value);
		void setAverage(double value);
		void addMeasurement(double value);
		double getAverage() const;

	private:

		double alpha;
		double averageValue;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(alpha),
				CEREAL_NVP(averageValue));
		}
	};
}
