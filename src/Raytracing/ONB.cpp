// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ONB.h"

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

ONB ONB::fromNormal(const Vector3& normal)
{
	Vector3 up = Vector3::UP;
	//Vector3 up = Vector3(0.001, 1.0, 0.001);

	Vector3 w = normal;
	Vector3 v = w.cross(up).normalized();
	Vector3 u = v.cross(w).normalized();

	return ONB(u, v, w);
}
