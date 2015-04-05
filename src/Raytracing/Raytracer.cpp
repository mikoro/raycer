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
	const double rayToLightOriginOffset = 0.000001;
}

void Raytracer::raytrace(const Framebuffer& framebuffer, const Scene& scene)
{
	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();
	int pixelCount = width * height;
	uint32_t* pixelData = framebuffer.getPixelData();
	int primitiveCount = (int)scene.primitives.size();
	int lightCount = (int)scene.lights.size();

	#pragma omp parallel for schedule(dynamic, 4096)
	for (int i = 0; i < pixelCount; ++i)
	{
		int x = i % width;
		int y = i / width;

		Ray rayToScene = scene.camera.getRay(x, y);

		for (int p = 0; p < primitiveCount; ++p)
			scene.primitives[p]->intersect(rayToScene);

		Color pixelColor;

		if (rayToScene.intersection.wasFound)
		{
			Color lightColor;

			for (int l = 0; l < lightCount; ++l)
			{
				const Light& light = scene.lights[l];
				Vector3 vectorToLight = light.position - rayToScene.intersection.position;
				Vector3 directionToLight = vectorToLight.normalized();
				double distanceToLight = vectorToLight.length();
				Ray rayToLight = Ray(rayToScene.intersection.position + directionToLight * rayToLightOriginOffset, directionToLight);

				for (int p = 0; p < primitiveCount; ++p)
					scene.primitives[p]->intersect(rayToLight);

				if (!rayToLight.intersection.wasFound || distanceToLight < rayToLight.intersection.distance)
				{
					double diffuseAmount = directionToLight.dot(rayToScene.intersection.normal);

					if (diffuseAmount > 0.0)
						lightColor += light.diffuseColor * diffuseAmount * rayToScene.intersection.material->diffuseReflectivity;
				}
			}

			pixelColor = rayToScene.intersection.material->color * lightColor;
		}

		pixelData[y * width + x] = pixelColor.clamped().getAbgrValue();
	}
}
