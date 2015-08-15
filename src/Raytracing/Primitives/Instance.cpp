// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Primitives/Instance.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"

using namespace Raycer;

void Instance::initialize()
{
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate.pitch, rotate.yaw, rotate.roll);
	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 translation = Matrix4x4::translate(translate);

	Matrix4x4 rotationInv = Matrix4x4::rotateXYZ(-rotate.pitch, -rotate.yaw, -rotate.roll);
	Matrix4x4 scalingInv = Matrix4x4::scale(scale.inversed());
	Matrix4x4 translationInv = Matrix4x4::translate(-translate);
	
	transformation = translation * scaling * rotation;
	transformationInv = rotationInv * scalingInv * translationInv;
	transformationInvT = transformationInv.transposed();
}

bool Instance::intersect(const Ray& ray, Intersection& intersection)
{
	Ray instanceRay = ray;
	instanceRay.origin = transformationInv.transformPosition(ray.origin);
	instanceRay.direction = transformationInv.transformDirection(ray.direction).normalized();
	instanceRay.update();

	if (primitive->intersect(instanceRay, intersection))
	{
		intersection.position = transformation.transformPosition(intersection.position);
		intersection.normal = transformationInvT.transformDirection(intersection.normal).normalized();
		intersection.primitive = this;

		return true;
	}

	return false;
}

AABB Instance::getAABB() const
{
	return primitive->getAABB().transformed(rotate, scale, translate);
}

Vector3* Instance::getPosition()
{
	return nullptr;
}
