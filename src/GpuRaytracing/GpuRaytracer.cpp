// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#include "GpuRaytracing/GpuRaytracer.h"
#include "GpuRaytracing/OpenCL.h"
#include "CpuRaytracing/Scene.h"
#include "Rendering/Image.h"

using namespace Raycer;

GpuRaytracer::GpuRaytracer()
{
}

GpuRaytracer::~GpuRaytracer()
{
}

void GpuRaytracer::initialize()
{
}

void GpuRaytracer::setSize(int width_, int height_)
{
	(void)width_;
	(void)height_;
}

void GpuRaytracer::trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount)
{
	(void)scene;
	(void)interrupted;
	(void)pixelCount;
	(void)rayCount;
}

Image GpuRaytracer::getImage()
{
	Image image;
	return image;
}
