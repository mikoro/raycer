// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Ray;

	class Shape
	{
	public:

		virtual ~Shape() {}

		virtual void intersects(const Ray& ray) = 0;
	};
}
