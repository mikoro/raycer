// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Sphere.h"

using namespace Raycer;

Sphere::Sphere()
{

}

Sphere::Sphere(const Vector3& position_, float radius_, const Material& material_) : position(position_), radius(radius_), material(material_)
{

}
