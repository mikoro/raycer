// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Math/Vertex3.h"

using namespace Raycer;

Vertex3::Vertex3(double x_, double y_, double z_) : x(x_), y(y_), z(z_)
{
}

Vertex3::Vertex3(const Vertex3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vertex3& Vertex3::operator=(const Vertex3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}
