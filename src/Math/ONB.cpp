// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/ONB.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

const ONB ONB::UP = ONB(Vector3(1.0, 0.0, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, 1.0, 0.0));

ONB::ONB()
{
}

ONB::ONB(const Vector3& u_, const Vector3& v_, const Vector3& w_) : u(u_), v(v_), w(w_)
{
}

ONB ONB::transformed(const Matrix4x4& tranformation) const
{
	ONB result;

	result.u = tranformation.transformDirection(u).normalized();
	result.v = tranformation.transformDirection(v).normalized();
	result.w = tranformation.transformDirection(w).normalized();

	return result;
}

ONB ONB::fromNormal(const Vector3& normal)
{
	Vector3 u = normal.cross(Vector3::ALMOST_UP).normalized();
	Vector3 v = u.cross(normal).normalized();
	Vector3 w = normal;

	return ONB(u, v, w);
}
