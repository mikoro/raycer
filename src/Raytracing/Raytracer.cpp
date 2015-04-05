// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Raytracing/Raytracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Framebuffer/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void Raytracer::raytrace(const Framebuffer& framebuffer, const Scene& scene)
{
	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();
	int pixelCount = width * height;
	uint32_t* pixelData = framebuffer.getPixelData();
	auto primitive = scene.primitives.at(0);
	auto light = scene.lights.at(0);

#pragma omp parallel for schedule(dynamic, 4096)
	for (int i = 0; i < pixelCount; ++i)
	{
		int y = i / width;
		int x = i % width;

		Intersection intersection;
		Intersection intersection2;

		Ray ray = scene.camera.getRay(x, y);

		if (primitive->intersects(ray, intersection))
		{
			Vector3 intersectionToLight = (light.position - intersection.position).normalized();

			Ray ray2 = Ray(intersection.position, intersectionToLight);
			bool lightBlocked = false;

			if (primitive->intersects(ray2, intersection2))
			{
				lightBlocked = true;
			}

			Color lightColor;
			const Material& material = primitive->getMaterial();
			float diffuseAmount = intersectionToLight.dot(intersection.normal);

			if (lightBlocked || diffuseAmount < 0.0f)
			{
				lightColor = scene.ambientColor * material.ambientReflectivity;
			}
			else
			{
				lightColor =
					scene.ambientColor * material.ambientReflectivity +
					light.diffuseColor * diffuseAmount * material.diffuseReflectivity;
			}

			Color pixelColor = material.color * lightColor;
			pixelData[y * width + x] = pixelColor.clamped().getAbgrValue();
		}
	}
}
