// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <limits>

#include "glfw/glfw3.h"
#include "gl/glext.h"

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>

#include "GpuRaytracing/GpuRaytracer.h"
#include "App.h"
#include "Utils/Settings.h"
#include "GpuRaytracing/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "CpuRaytracing/Scene.h"
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

void GpuRaytracer::setSize(int width_, int height_)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	OpenCL& openCl = App::getOpenCL();

	width = (uint32_t)width_;
	height = (uint32_t)height_;
	totalPixels = width * height;

	cl_int status = 0;

	if (pixels != nullptr)
	{
		status = clReleaseMemObject(pixels);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not release OpenCL memory object");
	}

	if (settings.general.interactive)
	{
		pixels = clCreateFromGLTexture2D(openCl.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getGpuTextureId(), &status);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not create OpenCL image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixels = clCreateImage2D(openCl.context, CL_MEM_WRITE_ONLY, &imageFormat, width, height, 0, NULL, &status);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not create OpenCL image");
	}
}

void GpuRaytracer::trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount)
{
	(void)scene;
	(void)interrupted;
	(void)pixelCount;
	(void)rayCount;

	Settings& settings = App::getSettings();
	OpenCL& openCl = App::getOpenCL();

	cl_int status;

	if (settings.general.interactive)
	{
		glFinish();
		status = clEnqueueAcquireGLObjects(openCl.commandQueue, 1, &pixels, 0, NULL, NULL);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not enqueue OpenCL GL object acquire");
	}

	status = clSetKernelArg(openCl.raytraceKernel, 0, sizeof(cl_mem), &pixels);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not set OpenCL kernel argument");

	const size_t globalSizes[] = { width, height };
	const size_t localSizes[] = { 8, 8 };

	status = clEnqueueNDRangeKernel(openCl.commandQueue, openCl.raytraceKernel, 2, NULL, &globalSizes[0], &localSizes[0], 0, NULL, NULL);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
	{
		status = clEnqueueReleaseGLObjects(openCl.commandQueue, 1, &pixels, 0, NULL, NULL);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not enqueue OpenCL GL object release");
	}

	clFinish(openCl.commandQueue);
}

Image GpuRaytracer::getImage()
{
	Image image;
	return image;
}
