﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/PathTracer.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Rendering/Image.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/MitchellFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"
#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Rendering/ToneMappers/ReinhardToneMapper.h"

using namespace Raycer;

PathTracer::PathTracer()
{
	std::random_device rd;
	generator.seed(rd());

	samplers[SamplerType::RANDOM] = std::unique_ptr<RandomSampler>(new RandomSampler());
	samplers[SamplerType::REGULAR] = std::unique_ptr<RegularSampler>(new RegularSampler());
	samplers[SamplerType::JITTERED] = std::unique_ptr<JitteredSampler>(new JitteredSampler());
	samplers[SamplerType::CMJ] = std::unique_ptr<CMJSampler>(new CMJSampler());

	filters[FilterType::BOX] = std::unique_ptr<BoxFilter>(new BoxFilter());
	filters[FilterType::TENT] = std::unique_ptr<TentFilter>(new TentFilter());
	filters[FilterType::BELL] = std::unique_ptr<BellFilter>(new BellFilter());
	filters[FilterType::MITCHELL] = std::unique_ptr<MitchellFilter>(new MitchellFilter());
	filters[FilterType::GAUSSIAN] = std::unique_ptr<GaussianFilter>(new GaussianFilter());
	filters[FilterType::LANCZOS_SINC] = std::unique_ptr<LanczosSincFilter>(new LanczosSincFilter());

	toneMappers[ToneMapperType::LINEAR] = std::unique_ptr<LinearToneMapper>(new LinearToneMapper());
	toneMappers[ToneMapperType::REINHARD] = std::unique_ptr<ReinhardToneMapper>(new ReinhardToneMapper());
}

void PathTracer::run(TracerState& state, std::atomic<bool>& interrupted)
{
	Scene& scene = *state.scene;
	Image& linearImage = *state.linearImage;
	Image& toneMappedImage = *state.toneMappedImage;

	#pragma omp parallel for schedule(dynamic, 1000)
	for (int pixelIndex = 0; pixelIndex < int(state.pixelCount); ++pixelIndex)
	{
		if (interrupted)
			continue;

		size_t offsetPixelIndex = size_t(pixelIndex) + state.pixelStartOffset;
		double x = double(offsetPixelIndex % state.imageWidth);
		double y = double(offsetPixelIndex / state.imageWidth);
		Vector2 pixelCoordinate = Vector2(x, y);

		Color pixelColor;

		Ray ray;
		bool isValidRay = scene.camera.getRay(pixelCoordinate, ray, 0.0);

		if (isValidRay)
			pixelColor = tracePath(scene, ray, 0, interrupted);

		linearImage.setPixel(size_t(pixelIndex), pixelColor);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
			state.pixelsProcessed += 100;
	}

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;

	toneMappers[scene.toneMapper.type]->apply(scene, linearImage, toneMappedImage);
}

Color PathTracer::tracePath(const Scene& scene, const Ray& ray, uint iteration, const std::atomic<bool>& interrupted)
{
	Color finalColor = Color::BLACK;

	if (interrupted)
		return finalColor;

	if (iteration >= scene.general.maxIterations)
		return finalColor;

	Intersection intersection;
	std::vector<Intersection> intersections;

	for (Primitive* primitive : scene.primitives.visible)
	{
		intersections.clear();
		primitive->intersect(ray, intersection, intersections);
	}

	if (!intersection.wasFound)
		return finalColor;

	//Material* material = intersection.primitive->material;

	return finalColor;
}
