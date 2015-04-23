// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <random>

#include "Raytracing/Raytracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Rendering/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	const double rayStartOffset = 0.000001;
}

void Raytracer::trace(RaytracerConfig& config, std::atomic<bool>& interrupted)
{
	Scene& scene = *config.scene;

	int rayCount = 0;
	int previousRayCount = 0;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> random(0.0, std::nextafter(1.0, DBL_MAX));

	#pragma omp parallel for schedule(dynamic, 1000) reduction(+:rayCount) firstprivate(previousRayCount)
	for (int pixelIndex = 0; pixelIndex < config.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + config.pixelOffset;
		double x = (double)(pixelOffsetIndex % config.sceneWidth);
		double y = (double)(pixelOffsetIndex / config.sceneWidth);

		Color finalColor(0.0, 0.0, 0.0, 1.0);
		double intersectionDistance = 0.0;

		if (scene.multisamples < 2)
		{
			double rx = x + 0.5;
			double ry = y + 0.5;

			Ray rayToScene = scene.camera.getRay(rx, ry);
			shootRay(config, rayToScene, rayCount, interrupted);
			finalColor = rayToScene.color;
			intersectionDistance = rayToScene.intersection.distance;
		}
		else
		{
			for (int i = 0; i < scene.multisamples; ++i)
			{
				for (int j = 0; j < scene.multisamples; ++j)
				{
					double rx = x + ((double)j + random(mt)) / (double)scene.multisamples;
					double ry = y + ((double)i + random(mt)) / (double)scene.multisamples;

					Ray rayToScene = scene.camera.getRay(rx, ry);
					shootRay(config, rayToScene, rayCount, interrupted);
					finalColor += rayToScene.color;
					intersectionDistance += rayToScene.intersection.distance;
				}
			}

			double multisamples2 = scene.multisamples * scene.multisamples;
			finalColor /= multisamples2;
			intersectionDistance /= multisamples2;
		}

		if (scene.fogEnabled)
		{
			double t = intersectionDistance / scene.fogDistance;
			t = std::max(0.0, std::min(t, 1.0));
			t = pow(t, scene.fogSteepness);
			finalColor = Color::lerp(finalColor, scene.fogColor, t);
		}

		finalColor.a = 1.0;
		config.renderTarget->setPixel(pixelIndex, finalColor.clamped());

		if ((pixelIndex + 1) % 100 == 0)
		{
			config.pixelsProcessed += 100;
			config.raysProcessed += (rayCount - previousRayCount);
			previousRayCount = rayCount;
		}
	}

	config.pixelsProcessed = config.pixelCount;
	config.raysProcessed = rayCount;
}

void Raytracer::shootRay(RaytracerConfig& config, Ray& ray, int& rayCount, std::atomic<bool>& interrupted)
{
	if (interrupted)
		return;

	Scene& scene = *config.scene;
	++rayCount;

	for (const Primitive* primitive : scene.primitiveList)
		primitive->intersect(ray);

	if (ray.intersection.wasFound)
	{
		Color combinedLightColor(0.0, 0.0, 0.0, 1.0);

		Material* material = scene.materialMap[ray.intersection.materialId];
		Texture* texture = scene.textureMap[material->textureId];

		if (material->reflectivity > 0.0 && ray.reflectionCount < scene.maxReflections)
		{
			Vector3 reflectionDirection = ray.direction.reflect(ray.intersection.normal);
			Ray reflectedRay = Ray(ray.intersection.position + reflectionDirection * rayStartOffset, reflectionDirection, ray.reflectionCount + 1);

			shootRay(config, reflectedRay, rayCount, interrupted);

			combinedLightColor = reflectedRay.color * material->reflectivity;
		}

		for (const Light& light : scene.lights)
		{
			Vector3 vectorToLight = light.position - ray.intersection.position;
			Vector3 directionToLight = vectorToLight.normalized();
			double distanceToLight = vectorToLight.length();
			Ray rayToLight = Ray(ray.intersection.position + directionToLight * rayStartOffset, directionToLight);

			for (const Primitive* primitive : scene.primitiveList)
				primitive->intersect(rayToLight);

			if (!rayToLight.intersection.wasFound || distanceToLight < rayToLight.intersection.distance)
			{
				double diffuseAmount = directionToLight.dot(ray.intersection.normal);

				if (diffuseAmount > 0.0)
				{
					combinedLightColor += light.color * light.intensity * diffuseAmount * material->diffuseness;

					Vector3 lightReflectionDirection = (2.0 * diffuseAmount * ray.intersection.normal) - directionToLight;
					double specularAmount = lightReflectionDirection.dot(-ray.direction);

					if (specularAmount > 0.0)
					{
						specularAmount = pow(specularAmount, material->shininess);
						combinedLightColor += light.color * light.intensity * specularAmount * material->specularity;
					}
				}
			}
		}

		ray.color = combinedLightColor * texture->getColor(ray.intersection.position, ray.intersection.texcoord);
	}
}
