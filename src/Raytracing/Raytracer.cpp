// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#include "Raytracing/Raytracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Framebuffer/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	const double rayStartOffset = 0.000001;
}

void Raytracer::raytrace(const Framebuffer& framebuffer, const Scene& scene)
{
	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();
	int pixelCount = width * height;
	uint32_t* pixelData = framebuffer.getPixelData();

	#pragma omp parallel for schedule(dynamic, 4096)
	for (int i = 0; i < pixelCount; ++i)
	{
		int x = i % width;
		int y = i / width;

		Ray rayToScene = scene.camera.getRay(x, y);
		shootRay(rayToScene, scene);
		pixelData[y * width + x] = rayToScene.color.clamped().getAbgrValue();
	}
}

void Raytracer::shootRay(Ray& ray, const Scene& scene)
{
	for (int p = 0; p < scene.primitives.size(); ++p)
		scene.primitives[p]->intersect(ray);

	if (ray.intersection.wasFound)
	{
		Color lightColor(0.0, 0.0, 0.0);

		if (ray.intersection.material->rayReflectivity > 0.0 && ray.reflectionCount < MAX_REFLECTIONS)
		{
			Vector3 reflectionDirection = ray.direction.reflect(ray.intersection.normal);
			Ray reflectedRay = Ray(ray.intersection.position + reflectionDirection * rayStartOffset, reflectionDirection, ray.reflectionCount + 1);

			shootRay(reflectedRay, scene);

			lightColor += reflectedRay.color * ray.intersection.material->rayReflectivity;
		}

		for (int l = 0; l < scene.lights.size(); ++l)
		{
			const Light& light = scene.lights[l];
			Vector3 vectorToLight = light.position - ray.intersection.position;
			Vector3 directionToLight = vectorToLight.normalized();
			double distanceToLight = vectorToLight.length();
			Ray rayToLight = Ray(ray.intersection.position + directionToLight * rayStartOffset, directionToLight);

			for (int p = 0; p < scene.primitives.size(); ++p)
				scene.primitives[p]->intersect(rayToLight);

			if (!rayToLight.intersection.wasFound || distanceToLight < rayToLight.intersection.distance)
			{
				double diffuseAmount = directionToLight.dot(ray.intersection.normal);

				if (diffuseAmount > 0.0)
				{
					lightColor += light.diffuseColor * diffuseAmount * ray.intersection.material->diffuseReflectivity;

					Vector3 lightReflectionDirection = (2.0 * diffuseAmount * ray.intersection.normal) - directionToLight;
					double specularAmount = lightReflectionDirection.dot(-ray.direction);

					if (specularAmount > 0.0)
					{
						specularAmount = pow(specularAmount, ray.intersection.material->shininess);
						lightColor += light.specularColor * specularAmount * ray.intersection.material->specularReflectivity;
					}
				}
			}
		}

		ray.color = lightColor * ray.intersection.material->color;
	}
}
