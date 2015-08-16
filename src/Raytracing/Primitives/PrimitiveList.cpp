// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Primitives/PrimitiveList.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"

using namespace Raycer;

PrimitiveList::PrimitiveList()
{
}

PrimitiveList::PrimitiveList(const std::vector<Primitive*>& primitives_)
{
	for (Primitive* primitive : primitives_)
		primitives.push_back(primitive);
}

void PrimitiveList::initialize()
{
}

bool PrimitiveList::intersect(const Ray& ray, Intersection& intersection)
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	bool wasFound = false;

	for (Primitive* primitive : primitives)
	{
		if (primitive->intersect(ray, intersection))
			wasFound = true;
	}

	return wasFound;
}

AABB PrimitiveList::getAABB() const
{
	AABB aabb;

	for (Primitive* primitive : primitives)
		aabb.expand(primitive->getAABB());

	return aabb;
}

void PrimitiveList::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	for (Primitive* primitive : primitives)
		primitive->transform(scale, rotate, translate);
}
