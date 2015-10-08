// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "OpenCL/CLTracer.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLStructs.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Scene.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/Image.h"
#include "Runners/WindowRunner.h"

using namespace Raycer;

namespace
{
	void releaseMemObject(cl_mem* object)
	{
		if (*object != nullptr)
		{
			CLManager::checkError(clReleaseMemObject(*object), "Could not release memory object");
			*object = nullptr;
		}
	}
}

CLTracer::CLTracer()
{
}

CLTracer::~CLTracer()
{
	releaseMemObject(&statePtr);
	releaseMemObject(&cameraPtr);
	releaseMemObject(&raytracerPtr);
	releaseMemObject(&toneMapperPtr);
	releaseMemObject(&simpleFogPtr);
	releaseMemObject(&materialsPtr);
	releaseMemObject(&ambientLightPtr);
	releaseMemObject(&directionalLightsPtr);
	releaseMemObject(&pointLightsPtr);
	releaseMemObject(&trianglesPtr);
	releaseMemObject(&bvhNodesPtr);
	releaseMemObject(&outputImagePtr);

	for (cl_mem textureImagePtr : textureImagePtrs)
		releaseMemObject(&textureImagePtr);

	releaseMemObject(&dummyTextureImagePtr);

	if (raytraceKernel != nullptr)
	{
		clReleaseKernel(raytraceKernel);
		raytraceKernel = nullptr;
	}

	if (raytraceProgram != nullptr)
	{
		clReleaseProgram(raytraceProgram);
		raytraceProgram = nullptr;
	}
}

void CLTracer::initialize(const Scene& scene)
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Initializing OpenCL raytracer");

	clScene.readSceneFull(scene);
	createBuffers();
	uploadFullData();

	std::vector<std::string> sourceFiles = {
		"data/opencl/common.cl",
		"data/opencl/structs.cl",
		"data/opencl/constructors.cl",
		"data/opencl/camera.cl",
		"data/opencl/textures.cl",
		"data/opencl/intersections.cl",
		"data/opencl/lighting.cl",
		"data/opencl/raytrace.cl"
	};

	raytraceProgram = clManager.createProgram(sourceFiles);
	raytraceKernel = clManager.createKernel(raytraceProgram, "raytrace");

	kernelArgumentIndex = 0;

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &statePtr), "Could not set kernel argument (state)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &cameraPtr), "Could not set kernel argument (camera)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &raytracerPtr), "Could not set kernel argument (raytracer)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &toneMapperPtr), "Could not set kernel argument (tone mapper)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &simpleFogPtr), "Could not set kernel argument (simple fog)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &materialsPtr), "Could not set kernel argument (materials)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &ambientLightPtr), "Could not set kernel argument (ambient light)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &directionalLightsPtr), "Could not set kernel argument (directional lights)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &pointLightsPtr), "Could not set kernel argument (point lights)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &trianglesPtr), "Could not set kernel argument (triangles)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &bvhNodesPtr), "Could not set kernel argument (bvh nodes)");

	outputImageArgumentIndex = kernelArgumentIndex++;
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(outputImageArgumentIndex), sizeof(cl_mem), &outputImagePtr), "Could not set kernel argument (output image)");

	createTextureImages();
	initialized = true;
}

void CLTracer::resizeImageBuffer(size_t width, size_t height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	imageBufferWidth = width;
	imageBufferHeight = height;

	cl_int status = 0;

	// use OpenGL texture as an image
	if (settings.general.interactive)
	{
		outputImagePtr = clCreateFromGLTexture2D(clManager.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getImageTextureId(), &status);
		CLManager::checkError(status, "Could not create output image from OpenGL texture");
	}
	else // create own image
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		outputImagePtr = clCreateImage2D(clManager.context, CL_MEM_WRITE_ONLY, &imageFormat, imageBufferWidth, imageBufferHeight, 0, nullptr, &status);
		CLManager::checkError(status, "Could not create output image");
	}

	if (initialized)
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(outputImageArgumentIndex), sizeof(cl_mem), &outputImagePtr), "Could not set kernel argument (output image)");
}

void CLTracer::releaseImageBuffer()
{
	releaseMemObject(&outputImagePtr);
}

void CLTracer::run(TracerState& state, std::atomic<bool>& interrupted)
{
	(void)interrupted;

	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();
	WindowRunner& windowRunner = App::getWindowRunner();

	clScene.readSceneCamera(*state.scene);

	if (settings.general.interactive)
		clScene.state.time = cl_float(windowRunner.getElapsedTime());
	else
		clScene.state.time = 1.0f;

	uploadCameraData();

	if (settings.general.interactive)
	{
		glFinish();
		CLManager::checkError(clEnqueueAcquireGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, nullptr, nullptr), "Could not enqueue OpenGL object acquire");
	}

	const size_t globalSizes[] = { imageBufferWidth, imageBufferHeight };

	CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, raytraceKernel, 2, nullptr, &globalSizes[0], nullptr, 0, nullptr, nullptr), "Could not enqueue raytrace kernel");

	if (settings.general.interactive)
		CLManager::checkError(clEnqueueReleaseGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, nullptr, nullptr), "Could not enqueue OpenGL object release");

	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");
}

Image CLTracer::downloadImage()
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Downloading image data from the OpenCL device");

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { imageBufferWidth, imageBufferHeight, 1 };

	std::vector<float> data(imageBufferWidth * imageBufferHeight * 4);

	cl_int status = clEnqueueReadImage(clManager.commandQueue, outputImagePtr, CL_TRUE, &origin[0], &region[0], 0, 0, &data[0], 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not read output image buffer");

	return Image(imageBufferWidth, imageBufferHeight, &data[0]);
}

void CLTracer::createBuffers()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	statePtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::State), nullptr, &status);
	CLManager::checkError(status, "Could not create state buffer");

	cameraPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Camera), nullptr, &status);
	CLManager::checkError(status, "Could not create camera buffer");

	raytracerPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Raytracer), nullptr, &status);
	CLManager::checkError(status, "Could not create raytracer buffer");

	toneMapperPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::ToneMapper), nullptr, &status);
	CLManager::checkError(status, "Could not create tone mapper buffer");

	simpleFogPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::SimpleFog), nullptr, &status);
	CLManager::checkError(status, "Could not create simple fog buffer");

	if (clScene.materials.size() > 0)
	{
		materialsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Material) * clScene.materials.size(), nullptr, &status);
		CLManager::checkError(status, "Could not create materials buffer");
	}

	ambientLightPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::AmbientLight), nullptr, &status);
	CLManager::checkError(status, "Could not create ambient light buffer");

	if (clScene.directionalLights.size() > 0)
	{
		directionalLightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::DirectionalLight) * clScene.directionalLights.size(), nullptr, &status);
		CLManager::checkError(status, "Could not create directional lights buffer");
	}

	if (clScene.pointLights.size() > 0)
	{
		pointLightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::PointLight) * clScene.pointLights.size(), nullptr, &status);
		CLManager::checkError(status, "Could not create point lights buffer");
	}

	if (clScene.triangles.size() > 0)
	{
		trianglesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Triangle) * clScene.triangles.size(), nullptr, &status);
		CLManager::checkError(status, "Could not create triangles buffer");
	}

	if (clScene.bvhNodes.size() > 0)
	{
		bvhNodesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::BVHNode) * clScene.bvhNodes.size(), nullptr, &status);
		CLManager::checkError(status, "Could not create bvh nodes buffer");
	}
}

void CLTracer::uploadFullData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status;

	status = clEnqueueWriteBuffer(clManager.commandQueue, statePtr, CL_FALSE, 0, sizeof(OpenCL::State), &clScene.state, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write state buffer");

	uploadCameraData();

	status = clEnqueueWriteBuffer(clManager.commandQueue, raytracerPtr, CL_FALSE, 0, sizeof(OpenCL::Raytracer), &clScene.raytracer, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write raytracer buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, toneMapperPtr, CL_FALSE, 0, sizeof(OpenCL::ToneMapper), &clScene.toneMapper, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write tone mapper buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, simpleFogPtr, CL_FALSE, 0, sizeof(OpenCL::SimpleFog), &clScene.simpleFog, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write simple fog buffer");

	if (clScene.materials.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, materialsPtr, CL_FALSE, 0, sizeof(OpenCL::Material) * clScene.materials.size(), &clScene.materials[0], 0, nullptr, nullptr);
		CLManager::checkError(status, "Could not write materials buffer");
	}

	status = clEnqueueWriteBuffer(clManager.commandQueue, ambientLightPtr, CL_FALSE, 0, sizeof(OpenCL::AmbientLight), &clScene.ambientLight, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write ambient light buffer");

	if (clScene.directionalLights.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, directionalLightsPtr, CL_FALSE, 0, sizeof(OpenCL::DirectionalLight) * clScene.directionalLights.size(), &clScene.directionalLights[0], 0, nullptr, nullptr);
		CLManager::checkError(status, "Could not write directional lights buffer");
	}

	if (clScene.pointLights.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, pointLightsPtr, CL_FALSE, 0, sizeof(OpenCL::PointLight) * clScene.pointLights.size(), &clScene.pointLights[0], 0, nullptr, nullptr);
		CLManager::checkError(status, "Could not write point lights buffer");
	}

	if (clScene.triangles.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, trianglesPtr, CL_FALSE, 0, sizeof(OpenCL::Triangle) * clScene.triangles.size(), &clScene.triangles[0], 0, nullptr, nullptr);
		CLManager::checkError(status, "Could not write triangles buffer");
	}

	if (clScene.bvhNodes.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, bvhNodesPtr, CL_FALSE, 0, sizeof(OpenCL::BVHNode) * clScene.bvhNodes.size(), &clScene.bvhNodes[0], 0, nullptr, nullptr);
		CLManager::checkError(status, "Could not write bvh nodes buffer");
	}

	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");
}

void CLTracer::uploadCameraData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status;

	status = clEnqueueWriteBuffer(clManager.commandQueue, cameraPtr, CL_FALSE, 0, sizeof(OpenCL::Camera), &clScene.camera, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write camera buffer");
}

void CLTracer::createTextureImages()
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	log.logInfo("Uploading texture data to the OpenCL device");

	cl_image_format imageFormat;
	imageFormat.image_channel_data_type = CL_FLOAT;
	imageFormat.image_channel_order = CL_RGBA;

	dummyTextureImagePtr = clCreateImage2D(clManager.context, CL_MEM_READ_ONLY, &imageFormat, 1, 1, 0, nullptr, &status);
	CLManager::checkError(status, "Could not create dummy texture image");

	const std::vector<Image>& images = ImagePool::getImages();
	size_t totalBytes = 0;

	for (size_t i = 0; i < KERNEL_TEXTURE_COUNT && i < images.size(); ++i)
	{
		const Image& image = images[i];

		size_t textureImageWidth = image.getWidth();
		size_t textureImageHeight = image.getHeight();

		cl_mem textureImagePtr = clCreateImage2D(clManager.context, CL_MEM_READ_ONLY, &imageFormat, textureImageWidth, textureImageHeight, 0, nullptr, &status);
		CLManager::checkError(status, "Could not create texture image");

		std::vector<float> floatPixelData = image.getFloatPixelData();
		totalBytes += floatPixelData.size() * sizeof(float);

		size_t origin[3] = { 0, 0, 0 };
		size_t region[3] = { textureImageWidth, textureImageHeight, 1 };

		status = clEnqueueWriteImage(clManager.commandQueue, textureImagePtr, CL_TRUE, &origin[0], &region[0], 0, 0, &floatPixelData[0], 0, nullptr, nullptr);

		if (status == CL_MEM_OBJECT_ALLOCATION_FAILURE)
		{
			log.logWarning("GPU video memory limit has been reached");
			break;
		}

		CLManager::checkError(status, "Could not write texture image buffer");
		textureImagePtrs.push_back(textureImagePtr);
	}

	log.logInfo("Total texture memory used: %.2f MB", float(totalBytes) / 1024.0 / 1024.0);

	for (size_t i = 0; i < textureImagePtrs.size(); ++i)
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &textureImagePtrs[i]), "Could not set kernel argument (texture image)");

	for (int64_t i = 0; i < (int64_t(KERNEL_TEXTURE_COUNT) - int64_t(textureImagePtrs.size())); ++i)
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(kernelArgumentIndex++), sizeof(cl_mem), &dummyTextureImagePtr), "Could not set kernel argument (dummy texture image)");
}
