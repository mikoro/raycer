// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <limits>

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Raytracing/Material.h"

namespace Raycer
{
	struct Intersection
	{
		bool wasFound = false;
		float distance = std::numeric_limits<float>::max();
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 normal = Vector3(0.0f, 0.0f, 0.0f);
		Vector2 textureCoordinate = Vector2(0.0f, 0.0f);
		const Material* material;
	};
}
