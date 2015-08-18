// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <limits>

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Raytracing/ONB.h"

namespace Raycer
{
	class Primitive;

	struct Intersection
	{
		bool wasFound = false;
		double distance = std::numeric_limits<double>::max();
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
		ONB onb;
		Primitive* primitive = nullptr;
	};
}
