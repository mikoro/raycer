// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/PathTracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

Color PathTracer::trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	return traceRecursive(scene, ray, 0, generator, interrupted);
}

Color PathTracer::traceRecursive(const Scene& scene, const Ray& ray, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	if (interrupted)
		return Color::BLACK;

	if (iteration >= scene.general.maxPathLength)
		return Color::BLACK;

	Intersection intersection;
	std::vector<Intersection> intersections;

	for (Primitive* primitive : scene.primitives.visible)
	{
		intersections.clear();
		primitive->intersect(ray, intersection, intersections);
	}

	if (!intersection.wasFound)
		return Color::BLACK;

	Material* material = intersection.primitive->material;
	Color emittance;

	if (material->emittanceMapTexture != nullptr)
		emittance = material->emittanceMapTexture->getColor(intersection.texcoord, intersection.position) * material->emittanceMapTexture->intensity;
	else
		emittance = material->emittance;

	if (!emittance.isZero())
		return emittance;

	Sampler* sampler = samplers[SamplerType::RANDOM].get();
	Vector3 newDirection = sampler->getHemisphereSample(intersection.onb, 0.0, 0, 0, 0, 0);

	Ray newRay;
	newRay.origin = intersection.position + newDirection * scene.general.rayStartOffset;
	newRay.direction = newDirection;
	newRay.precalculate();

	Color reflectance;

	if (material->reflectanceMapTexture != nullptr)
		reflectance = material->reflectanceMapTexture->getColor(intersection.texcoord, intersection.position) * material->reflectanceMapTexture->intensity;
	else
		reflectance = material->reflectance;

	double alpha = std::abs(newDirection.dot(intersection.normal));
	Color brdf = 2.0 * reflectance * alpha;
	Color reflected = traceRecursive(scene, newRay, iteration + 1, generator, interrupted);

	return brdf * reflected;
}
