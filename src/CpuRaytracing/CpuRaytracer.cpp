// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "CpuRaytracing/CpuRaytracer.h"
#include "CpuRaytracing/RaytraceInfo.h"
#include "CpuRaytracing/Scene.h"
#include "CpuRaytracing/Ray.h"
#include "CpuRaytracing/Intersection.h"
#include "CpuRaytracing/Material.h"
#include "Rendering/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "App.h"
#include "Utils/Settings.h"

using namespace Raycer;

namespace
{
	const double rayStartOffset = 0.000001;
}

void CpuRaytracer::trace(RaytraceInfo& info, std::atomic<bool>& interrupted)
{
	Settings& settings = App::getSettings();
	Scene& scene = *info.scene;

	#pragma omp parallel for schedule(dynamic, 4096)
	for (int pixelIndex = 0; pixelIndex < info.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + info.pixelOffset;
		int x = pixelOffsetIndex % info.sceneWidth;
		int y = pixelOffsetIndex / info.sceneWidth;

		Ray rayToScene = scene.camera.getRay(x, y);
		shootRay(rayToScene, scene, interrupted, info.raysProcessed);
		Color finalColor = rayToScene.color;

		if (scene.fogEnabled)
		{
			double t = rayToScene.intersection.distance / scene.fogDistance;
			t = std::max(0.0, std::min(t, 1.0));
			t = pow(t, scene.fogSteepness);
			finalColor = Color::lerp(finalColor, scene.fogColor, t);
		}

		info.renderTarget->setPixel(pixelIndex, finalColor.clamped());

		if (!settings.general.interactive)
			++info.pixelsProcessed;
	}
}

void CpuRaytracer::shootRay(Ray& ray, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& raysProcessed)
{
	if (interrupted)
		return;

	Settings& settings = App::getSettings();

	if (!settings.general.interactive)
		++raysProcessed;

	for (size_t p = 0; p < scene.primitives.size(); ++p)
		scene.primitives[p]->intersect(ray);

	if (ray.intersection.wasFound)
	{
		Color lightColor(0.0, 0.0, 0.0);

		if (ray.intersection.material->reflectivity > 0.0 && ray.reflectionCount < scene.maxReflections)
		{
			Vector3 reflectionDirection = ray.direction.reflect(ray.intersection.normal);
			Ray reflectedRay = Ray(ray.intersection.position + reflectionDirection * rayStartOffset, reflectionDirection, ray.reflectionCount + 1);

			shootRay(reflectedRay, scene, interrupted, raysProcessed);

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
