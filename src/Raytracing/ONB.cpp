// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ONB.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

const ONB ONB::UP = ONB(Vector3::RIGHT, Vector3::FORWARD, Vector3::UP);

ONB::ONB()
{
}

ONB::ONB(const Vector3& u_, const Vector3& v_, const Vector3& w_)
{
	u = u_;
	v = v_;
	w = w_;
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
	Vector3 w = normal;
	Vector3 v = w.cross(Vector3::ALMOST_UP).normalized();
	Vector3 u = v.cross(w).normalized();

	return ONB(u, v, w);
}
