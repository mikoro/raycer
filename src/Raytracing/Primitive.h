// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector2.h"

namespace Raycer
{
	struct Ray;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual void initialize() = 0;
		virtual void intersect(Ray& ray) const = 0;

		int materialId = 0;
		Vector2 texcoordScale = Vector2(1.0, 1.0);
	};
}
