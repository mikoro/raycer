// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/AABB.h"
#include "Raytracing/Ray.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;

AABB::AABB()
{
	min.x = min.y = min.z = std::numeric_limits<double>::max();
	max.x = max.y = max.z = std::numeric_limits<double>::lowest();
}

AABB AABB::createFromMinMax(const Vector3& min_, const Vector3& max_)
{
	AABB aabb;

	aabb.min = min_;
	aabb.max = max_;

	return aabb;
}

AABB AABB::createFromCenterExtent(const Vector3& center, const Vector3& extent)
{
	AABB aabb;

	aabb.min = center - extent / 2.0;
	aabb.max = center + extent / 2.0;

	return aabb;
}

AABB AABB::createFromVertices(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	Vector3 min_;

	min_.x = std::min(v0.x, std::min(v1.x, v2.x));
	min_.y = std::min(v0.y, std::min(v1.y, v2.y));
	min_.z = std::min(v0.z, std::min(v1.z, v2.z));

	Vector3 max_;

	max_.x = std::max(v0.x, std::max(v1.x, v2.x));
	max_.y = std::max(v0.y, std::max(v1.y, v2.y));
	max_.z = std::max(v0.z, std::max(v1.z, v2.z));

	return AABB::createFromMinMax(min_, max_);
}

// http://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
bool AABB::intersects(const Ray& ray) const
{
	double tx0 = (min.x - ray.origin.x) * ray.inverseDirection.x;
	double tx1 = (max.x - ray.origin.x) * ray.inverseDirection.x;

	double tmin = std::min(tx0, tx1);
	double tmax = std::max(tx0, tx1);

	double ty0 = (min.y - ray.origin.y) * ray.inverseDirection.y;
	double ty1 = (max.y - ray.origin.y) * ray.inverseDirection.y;

	tmin = std::max(tmin, std::min(ty0, ty1));
	tmax = std::min(tmax, std::max(ty0, ty1));

	double tz0 = (min.z - ray.origin.z) * ray.inverseDirection.z;
	double tz1 = (max.z - ray.origin.z) * ray.inverseDirection.z;

	tmin = std::max(tmin, std::min(tz0, tz1));
	tmax = std::min(tmax, std::max(tz0, tz1));

	if (tmax < std::max(tmin, 0.0))
		return false;

	return true;
}

void AABB::expand(const AABB& other)
{
	min.x = std::min(min.x, other.min.x);
	min.y = std::min(min.y, other.min.y);
	min.z = std::min(min.z, other.min.z);

	max.x = std::max(max.x, other.max.x);
	max.y = std::max(max.y, other.max.y);
	max.z = std::max(max.z, other.max.z);
}

uint64_t AABB::getLargestAxis() const
{
	Vector3 extent = getExtent();
	uint64_t largest = 0;

	if (extent.y > extent.x)
		largest = 1;

	if (extent.z > extent.x && extent.z > extent.y)
		largest = 2;

	return largest;
}

AABB AABB::transformed(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) const
{
	Vector3 corners[8], newMin, newMax;
	Vector3 center = getCenter();

	corners[0] = min;
	corners[1] = Vector3(max.x, min.y, min.z);
	corners[2] = Vector3(max.x, min.y, max.z);
	corners[3] = Vector3(min.x, min.y, max.z);
	corners[4] = max;
	corners[5] = Vector3(min.x, max.y, max.z);
	corners[6] = Vector3(min.x, max.y, min.z);
	corners[7] = Vector3(max.x, max.y, min.z);

	newMin.x = newMin.y = newMin.z = std::numeric_limits<double>::max();
	newMax.x = newMax.y = newMax.z = std::numeric_limits<double>::lowest();

	Matrix4x4 scaling = Matrix4x4::scale(scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(rotate);
	Matrix4x4 translation1 = Matrix4x4::translate(-center);
	Matrix4x4 translation2 = Matrix4x4::translate(center + translate);
	Matrix4x4 transformation = translation2 * rotation * scaling * translation1;

	for (auto & corner : corners)
	{
		corner = transformation.transformPosition(corner);

		newMin.x = std::min(newMin.x, corner.x);
		newMin.y = std::min(newMin.y, corner.y);
		newMin.z = std::min(newMin.z, corner.z);

		newMax.x = std::max(newMax.x, corner.x);
		newMax.y = std::max(newMax.y, corner.y);
		newMax.z = std::max(newMax.z, corner.z);
	}

	return AABB::createFromMinMax(newMin, newMax);
}

Vector3 AABB::getMin() const
{
	return min;
}

Vector3 AABB::getMax() const
{
	return max;
}

Vector3 AABB::getCenter() const
{
	return (min + max) * 0.5;
}

Vector3 AABB::getExtent() const
{
	return max - min;
}

double AABB::getSurfaceArea() const
{
	Vector3 extent = getExtent();
	return 2.0 * (extent.x * extent.y + extent.z * extent.y + extent.x * extent.z);
}
