// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <limits>

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Raytracing/ONB.h"
#include "Raytracing/Primitives/CSG.h"

namespace Raycer
{
	class Primitive;

	struct Intersection
	{
		bool wasFound = false;
		double distance = std::numeric_limits<double>::max();
		Vector3 position;
		Vector3 normal;
		ONB onb;
		Vector2 texcoord;
		CSGDirection direction = CSGDirection::IN;
		Primitive* primitive = nullptr;
		Primitive* instancePrimitive = nullptr;
	};
}
