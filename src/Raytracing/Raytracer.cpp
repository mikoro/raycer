// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

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

void Raytracer::traceFast(RenderTarget& renderTarget, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount)
{
	int width = renderTarget.getWidth();
	int height = renderTarget.getHeight();
	int totalPixelCount = width * height;

	#pragma omp parallel for schedule(dynamic, 4096)
	for (int i = 0; i < totalPixelCount; ++i)
	{
		if (interrupted)
			continue;

		int x = i % width;
		int y = i / width;

		Ray rayToScene = scene.camera.getRay(x, y);
		shootRay(rayToScene, scene, interrupted, rayCount);
		Color finalColor = rayToScene.color;

		if (scene.fogEnabled)
		{
			double t = rayToScene.intersection.distance / scene.fogDistance;
			t = std::max(0.0, std::min(t, 1.0));
			t = pow(t, scene.fogSteepness);
			finalColor = Color::lerp(finalColor, scene.fogColor, t);
		}

		renderTarget.setPixel(x, y, finalColor.clamped());
		++pixelCount;
	}
}

void Raytracer::traceFull(RenderTarget& renderTarget, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount)
{
	(void)renderTarget;
	(void)scene;
	(void)interrupted;
	(void)pixelCount;
	(void)rayCount;
}

void Raytracer::shootRay(Ray& ray, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& rayCount)
{
	if (interrupted)
		return;

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

			shootRay(reflectedRay, scene, interrupted, rayCount);

			lightColor += reflectedRay.color * ray.intersection.material->reflectivity;
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
