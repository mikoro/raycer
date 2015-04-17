// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "CpuRaytracing/CpuRaytracer.h"
#include "CpuRaytracing/Scene.h"
#include "CpuRaytracing/Ray.h"
#include "CpuRaytracing/Intersection.h"
#include "CpuRaytracing/Material.h"
#include "Rendering/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	const double rayStartOffset = 0.000001;
}

void CpuRaytracer::trace(CpuRaytracerConfig& config, std::atomic<bool>& interrupted)
{
	Scene& scene = *config.scene;
	int rayCount = 0;
	int previousRayCount = 0;

	#pragma omp parallel for schedule(dynamic, 1000) reduction(+:rayCount) firstprivate(previousRayCount)
	for (int pixelIndex = 0; pixelIndex < config.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + config.pixelOffset;
		int x = pixelOffsetIndex % config.sceneWidth;
		int y = pixelOffsetIndex / config.sceneWidth;

		Ray rayToScene = scene.camera.getRay(x, y);
		shootRay(config, rayToScene, rayCount, interrupted);
		Color finalColor = rayToScene.color;

		if (scene.fogEnabled)
		{
			double t = rayToScene.intersection.distance / scene.fogDistance;
			t = std::max(0.0, std::min(t, 1.0));
			t = pow(t, scene.fogSteepness);
			finalColor = Color::lerp(finalColor, scene.fogColor, t);
		}

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

void CpuRaytracer::shootRay(CpuRaytracerConfig& config, Ray& ray, int& rayCount, std::atomic<bool>& interrupted)
{
	if (interrupted)
		return;

	Scene& scene = *config.scene;
	++rayCount;

	for (size_t p = 0; p < scene.primitives.size(); ++p)
		scene.primitives[p]->intersect(ray);

	if (ray.intersection.wasFound)
	{
		Color lightColor(0.0, 0.0, 0.0);

		if (ray.intersection.material->reflectivity > 0.0 && ray.reflectionCount < scene.maxReflections)
		{
			Vector3 reflectionDirection = ray.direction.reflect(ray.intersection.normal);
			Ray reflectedRay = Ray(ray.intersection.position + reflectionDirection * rayStartOffset, reflectionDirection, ray.reflectionCount + 1);

			shootRay(config, reflectedRay, rayCount, interrupted);

			lightColor = reflectedRay.color * ray.intersection.material->reflectivity;
		}

		for (size_t l = 0; l < scene.lights.size(); ++l)
		{
			const Light& light = scene.lights[l];
			Vector3 vectorToLight = light.position - ray.intersection.position;
			Vector3 directionToLight = vectorToLight.normalized();
			double distanceToLight = vectorToLight.length();
			Ray rayToLight = Ray(ray.intersection.position + directionToLight * rayStartOffset, directionToLight);

			for (size_t p = 0; p < scene.primitives.size(); ++p)
				scene.primitives[p]->intersect(rayToLight);

			if (!rayToLight.intersection.wasFound || distanceToLight < rayToLight.intersection.distance)
			{
				double diffuseAmount = directionToLight.dot(ray.intersection.normal);

				if (diffuseAmount > 0.0)
				{
					lightColor += light.color * light.intensity * diffuseAmount * ray.intersection.material->diffuseConstant;

					Vector3 lightReflectionDirection = (2.0 * diffuseAmount * ray.intersection.normal) - directionToLight;
					double specularAmount = lightReflectionDirection.dot(-ray.direction);

					if (specularAmount > 0.0)
					{
						specularAmount = pow(specularAmount, ray.intersection.material->shininess);
						lightColor += light.color * light.intensity * specularAmount * ray.intersection.material->specularConstant;
					}
				}
			}
		}

		ray.color = lightColor * ray.intersection.material->color;
	}
}
