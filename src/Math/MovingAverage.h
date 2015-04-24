// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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

		double alpha = 1.0;
		double averageValue = 0.0;
	};
}
