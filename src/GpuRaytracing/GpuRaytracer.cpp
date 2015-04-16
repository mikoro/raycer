// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "GpuRaytracing/GpuRaytracer.h"
#include "CpuRaytracing/CpuRaytracer.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "GpuRaytracing/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "CpuRaytracing/Scene.h"
#include "Rendering/Image.h"

using namespace Raycer;

void GpuRaytracer::trace(CpuRaytracerConfig& config, std::atomic<bool>& interrupted)
{
	(void)interrupted;
	
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();

	if (settings.general.interactive)
	{
		glFinish();
		checkCLError(clEnqueueAcquireGLObjects(openCL.commandQueue, 1, &openCL.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkCLError(clSetKernelArg(openCL.raytraceKernel, 0, sizeof(cl_mem), &openCL.pixels), "Could not set OpenCL kernel argument");

	const size_t globalSizes[] = { (size_t)config.sceneWidth, (size_t)config.sceneHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkCLError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkCLError(clEnqueueReleaseGLObjects(openCL.commandQueue, 1, &openCL.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkCLError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}
