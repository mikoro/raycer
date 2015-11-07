// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/PrimitiveGroup.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"

using namespace Raycer;

PrimitiveGroup::PrimitiveGroup()
{
}

PrimitiveGroup::PrimitiveGroup(const std::vector<Primitive*>& primitives_)
{
	for (Primitive* primitive : primitives_)
		primitives.push_back(primitive);
}

void Raycer::PrimitiveGroup::initialize(const Scene& scene)
{
	if (enableBVH)
	{
		if (bvh.hasBeenBuilt)
			bvh.restore(scene);
		else
			bvh.build(primitives, bvhBuildInfo);
	}
	else
	{
		for (Primitive* primitive : primitives)
			aabb.expand(primitive->getAABB());
	}
}

bool PrimitiveGroup::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	if (enableBVH)
		return bvh.intersect(ray, intersection, intersections);
	else
	{
		bool wasFound = false;

		for (Primitive* primitive : primitives)
		{
			if (primitive->intersect(ray, intersection, intersections))
				wasFound = true;
		}

		return wasFound;
	}
}

AABB PrimitiveGroup::getAABB() const
{
	if (enableBVH)
		return bvh.getAABB();
	else
		return aabb;
}

void PrimitiveGroup::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	if (enableBVH)
		bvh.transform(scale, rotate, translate);
	else
	{
		aabb = AABB();

		for (Primitive* primitive : primitives)
		{
			primitive->transform(scale, rotate, translate);
			aabb.expand(primitive->getAABB());
		}
	}
}
