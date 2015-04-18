// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "GpuRaytracing/GpuRaytracer.h"
#include "Raytracing/Raytracer.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "Utils/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"

using namespace Raycer;

GpuRaytracer::GpuRaytracer()
{
}

GpuRaytracer::~GpuRaytracer()
{
	if (pixels != nullptr)
	{
		clReleaseMemObject(pixels);
		pixels = nullptr;
	}
}

void GpuRaytracer::trace(RaytracerConfig& config, std::atomic<bool>& interrupted)
{
	(void)interrupted;
	
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();

	if (settings.general.interactive)
	{
		glFinish();
		checkCLError(clEnqueueAcquireGLObjects(openCL.commandQueue, 1, &pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkCLError(clSetKernelArg(openCL.raytraceKernel, 0, sizeof(cl_mem), &pixels), "Could not set OpenCL kernel argument");

	const size_t globalSizes[] = { (size_t)config.sceneWidth, (size_t)config.sceneHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkCLError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkCLError(clEnqueueReleaseGLObjects(openCL.commandQueue, 1, &pixels, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkCLError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}

void GpuRaytracer::runTestKernel()
{
	OpenCL& openCL = App::getOpenCL();

	const size_t globalSize = 1;
	checkCLError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.testKernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	checkCLError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}

void GpuRaytracer::resize(int width_, int height_)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	OpenCL& openCL = App::getOpenCL();

	width = width_;
	height = height_;

	release();
	cl_int status = 0;

	if (settings.general.interactive)
	{
		pixels = clCreateFromGLTexture2D(openCL.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getGpuTextureId(), &status);
		checkCLError(status, "Could not create OpenCL image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixels = clCreateImage2D(openCL.context, CL_MEM_WRITE_ONLY, &imageFormat, width, height, 0, NULL, &status);
		checkCLError(status, "Could not create OpenCL image");
	}
}

void GpuRaytracer::release()
{
	if (pixels != nullptr)
	{
		checkCLError(clReleaseMemObject(pixels), "Could not release OpenCL memory object");
		pixels = nullptr;
	}
}

void GpuRaytracer::readImage()
{
	OpenCL& openCL = App::getOpenCL();

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { width, height, 1 };

	std::vector<float> pixelData(width * height * 4);

	cl_int status = clEnqueueReadImage(openCL.commandQueue, pixels, CL_TRUE, &origin[0], &region[0], 0, 0, &pixelData[0], 0, NULL, NULL);
	checkCLError(status, "Could not read OpenCL image buffer");

	image.setSize(width, height);
	int length = width * height * 4;

	for (int i = 0; i < length; i += 4)
	{
		float r = pixelData[i];
		float g = pixelData[i + 1];
		float b = pixelData[i + 2];
		float a = pixelData[i + 3];

		image.setPixel(i / 4, Color(r, g, b, a).clamped());
	}
}

Image& GpuRaytracer::getImage()
{
	return image;
}
