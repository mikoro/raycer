// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include "glfw/glfw3.h"
#include "gl/glext.h"

#include "GpuRaytracing/GpuRaytracer.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "GpuRaytracing/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "CpuRaytracing/Scene.h"
#include "Rendering/Image.h"

using namespace Raycer;

void GpuRaytracer::setSize(size_t width_, size_t height_)
{
	width = width_;
	height = height_;
}

void GpuRaytracer::trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<size_t>& pixelCount, std::atomic<size_t>& rayCount)
{
	(void)scene;
	(void)interrupted;
	(void)pixelCount;
	(void)rayCount;

	Settings& settings = App::getSettings();
	OpenCL& openCl = App::getOpenCL();

	if (settings.general.interactive)
	{
		glFinish();
		checkClError(clEnqueueAcquireGLObjects(openCl.commandQueue, 1, &openCl.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkClError(clSetKernelArg(openCl.raytraceKernel, 0, sizeof(cl_mem), &openCl.pixels), "Could not set OpenCL kernel argument");

	const size_t globalSizes[] = { width, height };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkClError(clEnqueueNDRangeKernel(openCl.commandQueue, openCl.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkClError(clEnqueueReleaseGLObjects(openCl.commandQueue, 1, &openCl.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkClError(clFinish(openCl.commandQueue), "Could not finish OpenCL command queue");
}

Image GpuRaytracer::getImage()
{
	Image image;
	return image;
}
