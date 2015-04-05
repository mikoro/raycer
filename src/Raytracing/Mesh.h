// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitive.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Mesh : public Primitive
	{
	public:

		Mesh();
		
		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Material material;
	};
}
