// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Ray;
	struct Intersection;
	struct Material;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual bool intersects(const Ray& ray, Intersection& intersection) const = 0;
		virtual const Material& getMaterial() const = 0;
	};
}
