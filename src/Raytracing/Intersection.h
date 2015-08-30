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

	enum class IntersectionDirection { DIR_IN, DIR_OUT };

	struct Intersection
	{
		bool wasFound = false;
		double distance = std::numeric_limits<double>::max();
		Primitive* primitive = nullptr;
		Vector3 position;
		Vector3 normal;
		ONB onb;
		Vector2 texcoord;
		IntersectionDirection direction = IntersectionDirection::DIR_IN;
	};
}
