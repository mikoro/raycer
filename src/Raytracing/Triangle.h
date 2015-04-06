// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitive.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Triangle : public Primitive
	{
	public:

		Triangle();
		
		Material material;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(material));
		}
	};
}
