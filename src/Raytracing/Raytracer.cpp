// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Pixel.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

Raytracer::Raytracer()
{
	std::random_device rd;
	mt.seed(rd());
	random = std::uniform_real_distribution<double>(0.0, 1.0);
}

void Raytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	Image& image = *state.image;
	Scene& scene = *state.scene;

	int rayCount = 0;
	int previousRayCount = 0;

	#pragma omp parallel for schedule(dynamic, 1000) reduction(+:rayCount) firstprivate(previousRayCount)
	for (int pixelIndex = 0; pixelIndex < state.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + state.pixelOffset;
		double x = (double)(pixelOffsetIndex % state.sceneWidth);
		double y = (double)(pixelOffsetIndex / state.sceneWidth);

		Pixel pixel = shootRays(scene, x, y, rayCount, interrupted);
		
		if (scene.fog.enabled)
			pixel.color = scene.fog.calculate(scene, pixel);

		image.setPixel(pixelIndex, pixel.color);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
		{
			state.pixelsProcessed += 100;
			state.raysProcessed += (rayCount - previousRayCount);
			previousRayCount = rayCount;
		}
	}

	state.raysProcessed = rayCount;

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;
}

Pixel Raytracer::shootRays(Scene& scene, double x, double y, int& rayCount, std::atomic<bool>& interrupted)
{
	Pixel pixel;

	if (scene.multisampler.type == MultisampleType::NONE)
	{
		double rx = x + 0.5;
		double ry = y + 0.5;

		Ray ray = scene.camera.getRay(rx, ry);
		traceRay(scene, ray, rayCount, interrupted);

		pixel.color = ray.color;
		pixel.position = ray.intersection.position;
		pixel.distance = ray.intersection.distance;

		return pixel;
	}
	
	int multisamples = scene.multisampler.multisamples;
	double multisamples2 = multisamples * multisamples;
	
	if (scene.multisampler.type == MultisampleType::UNIFORM || scene.multisampler.type == MultisampleType::REGULAR || scene.multisampler.type == MultisampleType::JITTER)
	{
		for (int i = 0; i < multisamples; ++i)
		{
			for (int j = 0; j < multisamples; ++j)
			{
				double rx = 0.0;
				double ry = 0.0;

				if (scene.multisampler.type == MultisampleType::UNIFORM)
				{
					rx = x + random(mt);
					ry = y + random(mt);
				}
				else if (scene.multisampler.type == MultisampleType::REGULAR)
				{
					rx = x + ((double)j + 0.5) / (double)multisamples;
					ry = y + ((double)i + 0.5) / (double)multisamples;
				}
				else if (scene.multisampler.type == MultisampleType::JITTER)
				{
					rx = x + ((double)j + random(mt)) / (double)multisamples;
					ry = y + ((double)i + random(mt)) / (double)multisamples;
				}

				Ray ray = scene.camera.getRay(rx, ry);
				traceRay(scene, ray, rayCount, interrupted);

				pixel.color += ray.color;
				pixel.position += ray.intersection.position;
				pixel.distance += ray.intersection.distance;
			}
		}
	}
	else if (scene.multisampler.type == MultisampleType::POISSON)
	{
	}

	pixel.color /= multisamples2;
	pixel.position /= multisamples2;
	pixel.distance /= multisamples2;

	return pixel;
}

void Raytracer::traceRay(Scene& scene, Ray& ray, int& rayCount, std::atomic<bool>& interrupted)
{
	if (interrupted)
		return;

	++rayCount;

	for (const Primitive* primitive : scene.primitivesList)
		primitive->intersect(ray);

	if (ray.intersection.wasFound)
	{
		Color reflectedColor(0.0, 0.0, 0.0);

		Material* material = scene.materialsMap[ray.intersection.materialId];
		Texture* texture = scene.texturesMap[material->textureId];

		if (material->reflectivity > 0.0 && ray.reflectionCount < scene.tracer.maxReflections)
		{
			Vector3 reflectionDirection = ray.direction.reflect(ray.intersection.normal);
			Ray reflectedRay = Ray(ray.intersection.position + reflectionDirection * rayStartOffset, reflectionDirection, ray.reflectionCount + 1);
			traceRay(scene, reflectedRay, rayCount, interrupted);
			reflectedColor = reflectedRay.color * material->reflectivity;
		}

		Color lightColor = calculateLighting(scene, ray, rayCount, interrupted);
		ray.color = (reflectedColor + lightColor) * texture->getColor(ray.intersection.position, ray.intersection.texcoord);
	}
}

Color Raytracer::calculateLighting(Scene& scene, Ray& ray, int& rayCount, std::atomic<bool>& interrupted)
{
	Color lightColor(0.0, 0.0, 0.0);

	Material* material = scene.materialsMap[ray.intersection.materialId];

	for (const Light& light : scene.lights)
	{
		if (interrupted)
			break;

		Vector3 vectorToLight = light.position - ray.intersection.position;
		Vector3 directionToLight = vectorToLight.normalized();
		double distanceToLight = vectorToLight.length();
		Ray rayToLight = Ray(ray.intersection.position + directionToLight * rayStartOffset, directionToLight);
		++rayCount;

		for (const Primitive* primitive : scene.primitivesList)
			primitive->intersect(rayToLight);

		if (!rayToLight.intersection.wasFound || distanceToLight < rayToLight.intersection.distance)
		{
			double diffuseAmount = directionToLight.dot(ray.intersection.normal);

			if (diffuseAmount > 0.0)
			{
				lightColor += light.color * light.intensity * diffuseAmount * material->diffuseness;

				Vector3 lightReflectionDirection = (2.0 * diffuseAmount * ray.intersection.normal) - directionToLight;
				double specularAmount = lightReflectionDirection.dot(-ray.direction);

				if (specularAmount > 0.0)
				{
					specularAmount = pow(specularAmount, material->shininess);
					lightColor += light.color * light.intensity * specularAmount * material->specularity;
				}
			}
		}
	}

	return lightColor;
}
