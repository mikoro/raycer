// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <limits>

#include "Raytracing/AABB.h"
#include "Raytracing/Ray.h"

using namespace Raycer;

AABB::AABB()
{
	min.x = min.y = min.z = std::numeric_limits<double>::max();
	max.x = max.y = max.z = std::numeric_limits<double>::lowest();
}

AABB::AABB(const Vector3& min_, const Vector3& max_)
{
	*this = createFromMinMax(min_, max_);
}

AABB AABB::createFromMinMax(const Vector3& min, const Vector3& max)
{
	AABB aabb;

	aabb.min = min;
	aabb.max = max;

	aabb.center = (min + max) * 0.5;
	aabb.extent = max - min;
	aabb.surfaceArea = 2.0 * (aabb.extent.x * aabb.extent.y + aabb.extent.z * aabb.extent.y + aabb.extent.x * aabb.extent.z);

	return aabb;
}

AABB AABB::createFromCenterExtent(const Vector3& center, const Vector3& extent)
{
	AABB aabb;

	aabb.center = center;
	aabb.extent = extent;

	aabb.min = center - extent;
	aabb.max = center + extent;

	aabb.surfaceArea = 2.0 * (extent.x * extent.y + extent.z * extent.y + extent.x * extent.z);

	return aabb;
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

	center = (min + max) * 0.5;
	extent = max - min;
	surfaceArea = 2.0 * (extent.x * extent.y + extent.z * extent.y + extent.x * extent.z);
}

int AABB::getLargestAxis() const
{
	int largest = 0;

	if (extent.y > extent.x)
		largest = 1;

	if (extent.z > extent.x && extent.z > extent.y)
		largest = 2;

	return largest;
}
