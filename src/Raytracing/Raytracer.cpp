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
	uint32_t* pixelData = framebuffer.getPixelData();
	Intersection intersection;

	for (int y = 0; y < height; ++y)
	{
		float ry = (float)y / (height - 1) - 0.5f;

		for (int x = 0; x < width; ++x)
		{
			float rx = (float)x / (width - 1) - 0.5f;

			Vector3 rayDirection = (Vector3(rx, ry, 0) - scene.camera.position).normalized();
			Ray ray = Ray(scene.camera.position, rayDirection);

			for (auto primitive : scene.primitives)
			{
				const Material& material = primitive->getMaterial();

				if (primitive->intersects(ray, intersection))
				{
					for (auto light : scene.lights)
					{
						Vector3 intersectionToLight = (light.position - intersection.position).normalized();
						float diffuseAmount = intersectionToLight.dot(intersection.normal);
						diffuseAmount = std::max(0.0f, diffuseAmount);

						Color lightColor =
							scene.ambientColor * material.ambientReflectivity +
							light.diffuseColor * diffuseAmount * material.diffuseReflectivity;

						Color pixelColor = material.color * lightColor;

						pixelData[y * width + x] = pixelColor.clamped().getAbgrValue();
					}
				}
			}
		}
	}
}
