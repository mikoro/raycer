// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	struct Ray;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual void intersect(Ray& ray) const = 0;
	};
}
