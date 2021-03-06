﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "OpenCL/CLTracer.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLStructs.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Scene.h"
#include "App.h"
#include "Settings.h"
#include "Utils/Log.h"
#include "Rendering/Image.h"
#include "Rendering/ImagePool.h"
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
	std::random_device rd;
	generator.seed(rd());
}

CLTracer::~CLTracer()
{
	releaseImageBuffers();
	releaseBuffers();
	releaseKernels();
}

void CLTracer::initializeKernels()
{
	CLManager& clManager = App::getCLManager();

	App::getLog().logInfo("Initializing OpenCL kernels");

	std::vector<std::string> sourceFiles =
	{
		"data/opencl/common.cl",
		"data/opencl/structs.cl",
		"data/opencl/random.cl",
		"data/opencl/sampler.cl",
		"data/opencl/constructors.cl",
		"data/opencl/camera.cl",
		"data/opencl/textures.cl",
		"data/opencl/intersections.cl",
		"data/opencl/lighting.cl",
		"data/opencl/raytrace.cl",
		"data/opencl/pathtrace.cl",
		"data/opencl/postprocess.cl"
	};

	program = clManager.createProgram(sourceFiles);
	raytraceKernel = clManager.createKernel(program, "raytrace");
	pathtraceKernel = clManager.createKernel(program, "pathtrace");
	postprocessKernel = clManager.createKernel(program, "postprocess");
}

void CLTracer::releaseKernels()
{
	if (raytraceKernel != nullptr)
	{
		clReleaseKernel(raytraceKernel);
		raytraceKernel = nullptr;
	}

	if (pathtraceKernel != nullptr)
	{
		clReleaseKernel(pathtraceKernel);
		pathtraceKernel = nullptr;
	}

	if (postprocessKernel != nullptr)
	{
		clReleaseKernel(postprocessKernel);
		postprocessKernel = nullptr;
	}

	if (program != nullptr)
	{
		clReleaseProgram(program);
		program = nullptr;
	}
}

void CLTracer::initializeImageBuffers(uint64_t width, uint64_t height, uint64_t imageTextureId)
{
	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();

	App::getLog().logInfo("Initializing OpenCL image buffers");

	imageWidth = width;
	imageHeight = height;
	imageLength = imageWidth * imageHeight;

	cl_int status = 0;

	seedsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_WRITE, sizeof(uint64_t) * imageLength, nullptr, &status);
	CLManager::checkError(status, "Could not create seeds buffer");

	cumulativeColorsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_WRITE, sizeof(cl_float4) * imageLength, nullptr, &status);
	CLManager::checkError(status, "Could not create cumulative colors buffer");

	filterWeightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageLength, nullptr, &status);
	CLManager::checkError(status, "Could not create filter weights buffer");
	
	if (settings.general.interactive) // use OpenGL texture as an image (assume size is right)
	{
		outputImagePtr = clCreateFromGLTexture2D(clManager.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, GLuint(imageTextureId), &status);
		CLManager::checkError(status, "Could not create output image from OpenGL texture");
	}
	else // create own image
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		outputImagePtr = clCreateImage2D(clManager.context, CL_MEM_WRITE_ONLY, &imageFormat, imageWidth, imageHeight, 0, nullptr, &status);
		CLManager::checkError(status, "Could not create output image");
	}

	if (buffersInitialized)
	{
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(seedsArgumentIndex), sizeof(cl_mem), &seedsPtr), "Could not set kernel argument (seeds)");
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(cumulativeColorsArgumentIndex), sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(filterWeightsArgumentIndex), sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");
		
		CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(seedsArgumentIndex), sizeof(cl_mem), &seedsPtr), "Could not set kernel argument (seeds)");
		CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(cumulativeColorsArgumentIndex), sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");
		CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(filterWeightsArgumentIndex), sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");

		CLManager::checkError(clSetKernelArg(postprocessKernel, 1, sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");
		CLManager::checkError(clSetKernelArg(postprocessKernel, 2, sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");
		CLManager::checkError(clSetKernelArg(postprocessKernel, 3, sizeof(cl_mem), &outputImagePtr), "Could not set kernel argument (output image)");
	}

	seeds.resize(imageLength);

	std::uniform_int_distribution<uint64_t> randomInt;

	for (uint64_t& seed : seeds)
		seed = randomInt(generator);

	status = clEnqueueWriteBuffer(clManager.commandQueue, seedsPtr, CL_TRUE, 0, sizeof(uint64_t) * imageLength, &seeds[0], 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write seeds buffer");

	clear();
}

void CLTracer::releaseImageBuffers()
{
	releaseMemObject(&seedsPtr);
	releaseMemObject(&cumulativeColorsPtr);
	releaseMemObject(&filterWeightsPtr);
	releaseMemObject(&outputImagePtr);
}

void CLTracer::initializeBuffers(const Scene& scene)
{
	if (buffersInitialized)
		releaseBuffers();

	App::getLog().logInfo("Initializing OpenCL buffers");

	clScene.readSceneFull(scene);
	createBuffers();
	uploadFullData();

	currentKernelArgumentIndex = 0;

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &statePtr), "Could not set kernel argument (state)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &statePtr), "Could not set kernel argument (state)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &generalPtr), "Could not set kernel argument (general)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &generalPtr), "Could not set kernel argument (general)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &cameraPtr), "Could not set kernel argument (camera)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &cameraPtr), "Could not set kernel argument (camera)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &toneMapperPtr), "Could not set kernel argument (tone mapper)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &toneMapperPtr), "Could not set kernel argument (tone mapper)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &simpleFogPtr), "Could not set kernel argument (simple fog)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &simpleFogPtr), "Could not set kernel argument (simple fog)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &materialsPtr), "Could not set kernel argument (materials)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &materialsPtr), "Could not set kernel argument (materials)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &ambientLightPtr), "Could not set kernel argument (ambient light)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &ambientLightPtr), "Could not set kernel argument (ambient light)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &directionalLightsPtr), "Could not set kernel argument (directional lights)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &directionalLightsPtr), "Could not set kernel argument (directional lights)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &pointLightsPtr), "Could not set kernel argument (point lights)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &pointLightsPtr), "Could not set kernel argument (point lights)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &trianglesPtr), "Could not set kernel argument (triangles)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &trianglesPtr), "Could not set kernel argument (triangles)");

	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &bvhNodesPtr), "Could not set kernel argument (bvh nodes)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &bvhNodesPtr), "Could not set kernel argument (bvh nodes)");

	seedsArgumentIndex = currentKernelArgumentIndex++;
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(seedsArgumentIndex), sizeof(cl_mem), &seedsPtr), "Could not set kernel argument (seeds)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(seedsArgumentIndex++), sizeof(cl_mem), &seedsPtr), "Could not set kernel argument (seeds)");

	cumulativeColorsArgumentIndex = currentKernelArgumentIndex++;
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(cumulativeColorsArgumentIndex), sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(cumulativeColorsArgumentIndex), sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");

	filterWeightsArgumentIndex = currentKernelArgumentIndex++;
	CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(filterWeightsArgumentIndex), sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");
	CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(filterWeightsArgumentIndex), sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");

	CLManager::checkError(clSetKernelArg(postprocessKernel, 0, sizeof(cl_mem), &statePtr), "Could not set kernel argument (state)");
	CLManager::checkError(clSetKernelArg(postprocessKernel, 1, sizeof(cl_mem), &cumulativeColorsPtr), "Could not set kernel argument (cumulative colors)");
	CLManager::checkError(clSetKernelArg(postprocessKernel, 2, sizeof(cl_mem), &filterWeightsPtr), "Could not set kernel argument (filter weights)");
	CLManager::checkError(clSetKernelArg(postprocessKernel, 3, sizeof(cl_mem), &outputImagePtr), "Could not set kernel argument (output image)");

	createTextureImages();
	buffersInitialized = true;
}

void CLTracer::releaseBuffers()
{
	releaseMemObject(&statePtr);
	releaseMemObject(&generalPtr);
	releaseMemObject(&cameraPtr);
	releaseMemObject(&toneMapperPtr);
	releaseMemObject(&simpleFogPtr);
	releaseMemObject(&materialsPtr);
	releaseMemObject(&ambientLightPtr);
	releaseMemObject(&directionalLightsPtr);
	releaseMemObject(&pointLightsPtr);
	releaseMemObject(&trianglesPtr);
	releaseMemObject(&bvhNodesPtr);

	for (cl_mem textureImagePtr : textureImagePtrs)
		releaseMemObject(&textureImagePtr);

	releaseMemObject(&dummyTextureImagePtr);

	buffersInitialized = false;
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

	clScene.state.imageWidth = cl_int(imageWidth);
	clScene.state.imageHeight = cl_int(imageHeight);

	uploadMinimalData();

	if (settings.general.interactive)
	{
		glFinish();
		CLManager::checkError(clEnqueueAcquireGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, nullptr, nullptr), "Could not enqueue OpenGL object acquire");
	}

	size_t globalSizes[] = { imageWidth, imageHeight };

	if (state.scene->general.tracerType == TracerType::RAY)
		CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, raytraceKernel, 2, nullptr, &globalSizes[0], nullptr, 0, nullptr, nullptr), "Could not enqueue raytrace kernel");
	else if (state.scene->general.tracerType == TracerType::PATH)
		CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, pathtraceKernel, 2, nullptr, &globalSizes[0], nullptr, 0, nullptr, nullptr), "Could not enqueue pathtrace kernel");
	else
		throw std::runtime_error("Invalid tracer type");

	CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, postprocessKernel, 2, nullptr, &globalSizes[0], nullptr, 0, nullptr, nullptr), "Could not enqueue generate image kernel");

	if (settings.general.interactive)
		CLManager::checkError(clEnqueueReleaseGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, nullptr, nullptr), "Could not enqueue OpenGL object release");

	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");

	state.pixelsProcessed = state.pixelCount;
}

Image CLTracer::downloadImage()
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Downloading image data from the OpenCL device");

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { imageWidth, imageHeight, 1 };

	std::vector<float> data(imageLength * 4);

	cl_int status = clEnqueueReadImage(clManager.commandQueue, outputImagePtr, CL_TRUE, &origin[0], &region[0], 0, 0, &data[0], 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not read output image buffer");

	return Image(imageWidth, imageHeight, &data[0]);
}

void CLTracer::clear()
{
	CLManager& clManager = App::getCLManager();

	cl_float pattern[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	CLManager::checkError(clEnqueueFillBuffer(clManager.commandQueue, cumulativeColorsPtr, pattern, sizeof(cl_float) * 4, 0, sizeof(cl_float) * 4 * imageLength, 0, nullptr, nullptr), "Could not enqueue fill buffer");
	CLManager::checkError(clEnqueueFillBuffer(clManager.commandQueue, filterWeightsPtr, pattern, sizeof(cl_float), 0, sizeof(cl_float) * imageLength, 0, nullptr, nullptr), "Could not enqueue fill buffer");
}

void CLTracer::createBuffers()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	statePtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::State), nullptr, &status);
	CLManager::checkError(status, "Could not create state buffer");

	generalPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::General), nullptr, &status);
	CLManager::checkError(status, "Could not create general buffer");

	cameraPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Camera), nullptr, &status);
	CLManager::checkError(status, "Could not create camera buffer");

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

	uploadMinimalData();

	status = clEnqueueWriteBuffer(clManager.commandQueue, generalPtr, CL_FALSE, 0, sizeof(OpenCL::General), &clScene.general, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write general buffer");

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

void CLTracer::uploadMinimalData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status;

	status = clEnqueueWriteBuffer(clManager.commandQueue, statePtr, CL_FALSE, 0, sizeof(OpenCL::State), &clScene.state, 0, nullptr, nullptr);
	CLManager::checkError(status, "Could not write state buffer");

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
	uint64_t totalBytes = 0;

	for (uint64_t i = 0; i < KERNEL_TEXTURE_COUNT && i < images.size(); ++i)
	{
		const Image& image = images[i];

		cl_mem textureImagePtr = clCreateImage2D(clManager.context, CL_MEM_READ_ONLY, &imageFormat, image.getWidth(), image.getHeight(), 0, nullptr, &status);
		CLManager::checkError(status, "Could not create texture image");

		totalBytes += image.getLength() * sizeof(float);

		size_t origin[3] = { 0, 0, 0 };
		size_t region[3] = { image.getWidth(), image.getHeight(), 1 };

		status = clEnqueueWriteImage(clManager.commandQueue, textureImagePtr, CL_TRUE, &origin[0], &region[0], 0, 0, &image.getPixelDataConst()[0], 0, nullptr, nullptr);

		if (status == CL_MEM_OBJECT_ALLOCATION_FAILURE)
		{
			log.logWarning("GPU video memory limit has been reached");
			break;
		}

		CLManager::checkError(status, "Could not write texture image buffer");
		textureImagePtrs.push_back(textureImagePtr);
	}

	log.logInfo("Total texture memory used: %.2f MB", float(totalBytes) / 1024.0 / 1024.0);

	for (uint64_t i = 0; i < textureImagePtrs.size(); ++i)
	{
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &textureImagePtrs[i]), "Could not set kernel argument (texture image)");
		CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &textureImagePtrs[i]), "Could not set kernel argument (texture image)");
	}

	for (int64_t i = 0; i < (int64_t(KERNEL_TEXTURE_COUNT) - int64_t(textureImagePtrs.size())); ++i)
	{
		CLManager::checkError(clSetKernelArg(raytraceKernel, cl_uint(currentKernelArgumentIndex), sizeof(cl_mem), &dummyTextureImagePtr), "Could not set kernel argument (dummy texture image)");
		CLManager::checkError(clSetKernelArg(pathtraceKernel, cl_uint(currentKernelArgumentIndex++), sizeof(cl_mem), &dummyTextureImagePtr), "Could not set kernel argument (dummy texture image)");
	}
}
