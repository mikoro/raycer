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

bool PrimitiveList::intersect(const Ray& ray, Intersection& intersection) const
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	bool wasFound = false;

	for (const Primitive* primitive : primitives)
	{
		if (primitive->intersect(ray, intersection))
			wasFound = true;
	}

	return wasFound;
}

AABB PrimitiveList::getAABB() const
{
	AABB aabb;

	for (const Primitive* primitive : primitives)
		aabb.expand(primitive->getAABB());

	return aabb;
}
