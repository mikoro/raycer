// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/Torus.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "Raytracing/Material.h"
#include "Raytracing/ONB.h"
#include "Math/EulerAngle.h"
#include "Math/Polynomial.h"

using namespace Raycer;

void Raycer::Torus::initialize(const Scene& scene)
{
	(void)scene;

	aabb = AABB::createFromMinMax(Vector3(-outerRadius - innerRadius, -outerRadius - innerRadius, innerRadius), Vector3(outerRadius + innerRadius, outerRadius + innerRadius, -innerRadius));
}

bool Torus::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	(void)intersections;

	if (ray.isShadowRay && material->nonShadowing)
		return false;

	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	double innerRadius2 = innerRadius * innerRadius;
	double outerRadius2 = outerRadius * outerRadius;

	double alpha = ray.direction.dot(ray.direction);
	double beta = 2.0 * (ray.origin.dot(ray.direction));
	double gamma = ray.origin.dot(ray.origin) - innerRadius2 - outerRadius2;

	double coefficients[5];
	coefficients[0] = alpha * alpha;
	coefficients[1] = 2.0 * alpha * beta;
	coefficients[2] = beta * beta + 2.0 * alpha * gamma + 4.0 * outerRadius2 * ray.direction.z * ray.direction.z;
	coefficients[3] = 2.0 * beta * gamma + 8.0 * outerRadius2 * ray.origin.z * ray.direction.z;
	coefficients[4] = gamma * gamma + 4.0 * outerRadius2 * ray.origin.z * ray.origin.z - 4.0 * outerRadius2 * innerRadius2;

	Polynomial<5> polynomial(coefficients);
	double t = std::numeric_limits<double>::max();

	if (!polynomial.findSmallestPositiveRealRoot(t, 32, 0.0001, 0.01))
		return false;

	if (t < ray.minDistance || t > ray.maxDistance)
		return false;

	if (t > intersection.distance)
		return false;

	Vector3 ip = ray.origin + (t * ray.direction);
	Vector3 normal;

	double temp = 4.0 * (ip.x * ip.x + ip.y * ip.y + ip.z * ip.z - innerRadius2 - outerRadius2);

	normal.x = ip.x * temp;
	normal.y = ip.y * temp;
	normal.z = ip.z * temp + 8.0 * outerRadius2 * ip.z;

	normal.normalize();

	intersection.wasFound = true;
	intersection.distance = t;
	intersection.primitive = this;
	intersection.position = ip;
	intersection.normal = material->invertNormal ? -normal : normal;
	intersection.onb = ONB::fromNormal(intersection.normal);

	return true;
}

AABB Torus::getAABB() const
{
	return aabb;
}

void Torus::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)rotate;
	(void)translate;

	outerRadius *= scale.x;
	innerRadius *= scale.x;

	aabb = AABB::createFromMinMax(Vector3(-outerRadius - innerRadius, -outerRadius - innerRadius, innerRadius), Vector3(outerRadius + innerRadius, outerRadius + innerRadius, -innerRadius));
}
