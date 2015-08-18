// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	class ONB
	{
	public:

		ONB();
		ONB(const Vector3& u, const Vector3& v, const Vector3& w);

		Vector3 u = Vector3(0.0, 0.0, 0.0);
		Vector3 v = Vector3(0.0, 0.0, 0.0);
		Vector3 w = Vector3(0.0, 0.0, 0.0);

		static ONB fromNormal(const Vector3& normal);

		static const ONB UP;
	};
}
