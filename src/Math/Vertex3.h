// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Vertex3
	{
	public:

		Vertex3(double x = 0.0, double y = 0.0, double z = 0.0);
		Vertex3(const Vertex3& v);
		Vertex3& operator=(const Vertex3& v);

		double x;
		double y;
		double z;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(x),
				CEREAL_NVP(y),
				CEREAL_NVP(z));
		}
	};
}
