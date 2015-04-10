// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#include "GpuRaytracing/GpuRaytracer.h"
#include "GpuRaytracing/OpenCLHelper.h"
#include "CpuRaytracing/Scene.h"
#include "Rendering/Image.h"

using namespace Raycer;

GpuRaytracer::GpuRaytracer(OpenCLHelper& helper_) : helper(helper_)
{

}

void GpuRaytracer::setSize(int width, int height)
{

}

void GpuRaytracer::trace(const Scene& scene)
{

}

Image& GpuRaytracer::getImage()
{
	Image image;
	return image;
}
