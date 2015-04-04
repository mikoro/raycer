// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Raytracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Framebuffer/Framebuffer.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void Raytracer::raytrace(const Framebuffer& framebuffer, const Scene& scene)
{
	int width = framebuffer.getWidth();
	int height = framebuffer.getHeight();
	uint32_t* pixelData = framebuffer.getPixelData();
	Vector3 cameraPosition = Vector3(0, 0, 4);

	for (int y = 0; y < height; ++y)
	{
		float ry = (float)y / (height - 1) - 0.5f;

		for (int x = 0; x < width; ++x)
		{
			float rx = (float)x / (width - 1) - 0.5f;

			Vector3 rayDirection = (Vector3(rx, ry, 0) - cameraPosition).normalized();
			Ray ray = Ray(cameraPosition, rayDirection);
			
			pixelData[y * width + x] = Color::RED.getAbgrValue();
		}
	}
}
