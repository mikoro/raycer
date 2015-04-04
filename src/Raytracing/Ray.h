// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	class Ray
	{
	public:

		Ray();
		Ray(const Vector3& origin, const Vector3& direction);
		
		Vector3 origin = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 direction = Vector3(0.0f, 0.0f, 0.0f);
		float intersectionDistance = 0.0f;
	};
}
