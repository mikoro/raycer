// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Instance.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"

using namespace Raycer;

void Instance::initialize()
{
	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate.pitch, rotate.yaw, rotate.roll);
	Matrix4x4 translation = Matrix4x4::translate(translate);
	transformation = translation * rotation * scaling;
	transformationInv = transformation.inverted();
	transformationInvT = transformationInv.transposed();
}

bool Instance::intersect(const Ray& ray, Intersection& intersection)
{
	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	Ray instanceRay = ray;
	Intersection instanceIntersection = intersection;

	instanceRay.origin = transformationInv.transformPosition(ray.origin);
	instanceRay.direction = transformationInv.transformDirection(ray.direction).normalized();
	instanceRay.update();

	if (primitive->intersect(instanceRay, instanceIntersection))
	{
		Vector3 position = transformation.transformPosition(instanceIntersection.position);
		
		double t = (position - ray.origin).length();

		if (t < ray.tmin || t > ray.tmax)
			return false;

		if (t > intersection.distance)
			return false;

		intersection.wasFound = true;
		intersection.distance = t;
		intersection.primitive = this;
		intersection.position = position;
		intersection.normal = transformationInvT.transformDirection(instanceIntersection.normal).normalized();
		intersection.texcoord = instanceIntersection.texcoord;

		return true;
	}

	return false;
}

AABB Instance::getAABB() const
{
	return primitive->getAABB().transformed(scale, rotate, translate);
}

Vector3* Instance::getPosition()
{
	return nullptr;
}
