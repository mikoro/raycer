// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "GpuRaytracing/GpuRaytracer.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "GpuRaytracing/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "CpuRaytracing/Scene.h"
#include "CpuRaytracing/RaytraceInfo.h"
#include "Rendering/Image.h"

using namespace Raycer;

void GpuRaytracer::trace(RaytraceInfo& info, std::atomic<bool>& interrupted)
{
	(void)interrupted;
	
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();

	if (settings.general.interactive)
	{
		glFinish();
		checkClError(clEnqueueAcquireGLObjects(openCL.commandQueue, 1, &openCL.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkClError(clSetKernelArg(openCL.raytraceKernel, 0, sizeof(cl_mem), &openCL.pixels), "Could not set OpenCL kernel argument");

	const size_t globalSizes[] = { info.sceneWidth, info.sceneHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkClError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkClError(clEnqueueReleaseGLObjects(openCL.commandQueue, 1, &openCL.pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkClError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}
