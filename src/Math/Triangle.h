// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Raycer
{
	class Triangle 
	{
	public:

		Vector3 vertices[3];
		Vector3 normals[3];
		Vector2 texcoords[3];
		Color colors[3];
	};
}
