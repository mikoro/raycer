// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/Tracer.h"
#include "Raytracing/Tracers/Raytracer.h"
#include "Raytracing/Tracers/PathTracer.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Math/Color.h"
#include "App.h"
#include "Settings.h"
#include "TracerState.h"
#include "Rendering/Film.h"
#include "Rendering/Samplers/CenterSampler.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Rendering/Samplers/PoissonDiscSampler.h"
#include "Rendering/Filters/BoxFilter.h"
#include "Rendering/Filters/TentFilter.h"
#include "Rendering/Filters/BellFilter.h"
#include "Rendering/Filters/GaussianFilter.h"
#include "Rendering/Filters/MitchellFilter.h"
#include "Rendering/Filters/LanczosSincFilter.h"

using namespace Raycer;

std::unique_ptr<Tracer> Tracer::getTracer(TracerType type)
{
	switch (type)
	{
		case TracerType::RAY: return std::make_unique<Raytracer>();
		case TracerType::PATH: return std::make_unique<PathTracer>();
		default: throw std::runtime_error("Invalid tracer type");
	}
}

Tracer::Tracer()
{
	samplers[SamplerType::CENTER] = std::make_unique<CenterSampler>();
	samplers[SamplerType::RANDOM] = std::make_unique<RandomSampler>();
	samplers[SamplerType::REGULAR] = std::make_unique<RegularSampler>();
	samplers[SamplerType::JITTERED] = std::make_unique<JitteredSampler>();
	samplers[SamplerType::CMJ] = std::make_unique<CMJSampler>();
	samplers[SamplerType::POISSON_DISC] = std::make_unique<PoissonDiscSampler>();

	filters[FilterType::BOX] = std::make_unique<BoxFilter>();
	filters[FilterType::TENT] = std::make_unique<TentFilter>();
	filters[FilterType::BELL] = std::make_unique<BellFilter>();
	filters[FilterType::GAUSSIAN] = std::make_unique<GaussianFilter>();
	filters[FilterType::MITCHELL] = std::make_unique<MitchellFilter>();
	filters[FilterType::LANCZOS_SINC] = std::make_unique<LanczosSincFilter>();
}

void Tracer::run(TracerState& state, std::atomic<bool>& interrupted)
{
	omp_set_num_threads(App::getSettings().general.maxThreadCount);
	uint64_t maxThreads = std::max(1, omp_get_max_threads());

	assert(maxThreads >= 1);

	if (maxThreads != generators.size())
	{
		generators.resize(maxThreads);
		std::random_device rd;

		for (auto& generator : generators)
			generator.seed(rd());
	}

	std::mutex ompThreadExceptionMutex;
	std::exception_ptr ompThreadException = nullptr;

	#pragma omp parallel for schedule(dynamic, 1000)
	for (int64_t pixelIndex = 0; pixelIndex < int64_t(state.pixelCount); ++pixelIndex)
	{
		try
		{
			if (interrupted)
				continue;

			uint64_t offsetPixelIndex = uint64_t(pixelIndex) + state.pixelStartOffset;
			double x = double(offsetPixelIndex % state.filmWidth);
			double y = double(offsetPixelIndex / state.filmWidth);
			Vector2 pixelCoordinate = Vector2(x, y);
			std::mt19937& generator = generators[omp_get_thread_num()];

			generateMultiSamples(*state.scene, *state.film, pixelCoordinate, uint64_t(pixelIndex), generator, interrupted);
			
			// progress reporting to another thread
			if ((pixelIndex + 1) % 100 == 0)
				state.pixelsProcessed += 100;
		}
		catch (...)
		{
			std::lock_guard<std::mutex> lock(ompThreadExceptionMutex);

			if (ompThreadException == nullptr)
				ompThreadException = std::current_exception();

			interrupted = true;
		}
	}

	if (ompThreadException != nullptr)
		std::rethrow_exception(ompThreadException);

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;
}

void Tracer::generateMultiSamples(const Scene& scene, Film& film, const Vector2& pixelCoordinate, uint64_t pixelIndex, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	assert(scene.general.multiSamplesSqrt >= 1);

	Sampler* sampler = samplers[scene.general.multiSamplerType].get();
	Filter* filter = filters[scene.general.multiSamplerFilterType].get();

	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);
	uint64_t n = scene.general.multiSamplesSqrt;

	for (uint64_t sy = 0; sy < n; ++sy)
	{
		for (uint64_t sx = 0; sx < n; ++sx)
		{
			Vector2 sampleOffset = sampler->getSample2D(sx, sy, n, n, permutation, generator);
			sampleOffset = (sampleOffset - Vector2(0.5, 0.5)) * 2.0 * filter->getRadius();
			double filterWeight = filter->getWeight(sampleOffset);
			Color sampledPixelColor = generateTimeSamples(scene, pixelCoordinate + sampleOffset, generator, interrupted);
			film.addSample(pixelIndex, sampledPixelColor, filterWeight);
		}
	}
}

Color Tracer::generateTimeSamples(const Scene& scene, const Vector2& pixelCoordinate, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	assert(scene.general.timeSamples >= 1);

	Sampler* sampler = samplers[scene.general.timeSamplerType].get();

	Color sampledPixelColor;
	uint64_t n = scene.general.timeSamples;

	for (uint64_t i = 0; i < n; ++i)
		sampledPixelColor += generateCameraSamples(scene, pixelCoordinate, sampler->getSample1D(i, n, 0, generator), generator, interrupted);

	return sampledPixelColor / double(n);
}

Color Tracer::generateCameraSamples(const Scene& scene, const Vector2& pixelCoordinate, double time, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	Ray ray;
	bool isValidRay = scene.camera.getRay(pixelCoordinate, ray, time);

	if (!isValidRay && scene.general.cameraSamplesSqrt == 0)
		return scene.general.offLensColor;

	if (scene.general.cameraSamplesSqrt == 0)
		return trace(scene, ray, generator, interrupted);

	Sampler* sampler = samplers[scene.general.cameraSamplerType].get();

	std::uniform_int_distribution<uint64_t> randomPermutation;
	uint64_t permutation = randomPermutation(generator);

	double apertureSize = scene.camera.apertureSize;
	double focalDistance = scene.camera.focalDistance;

	CameraState cameraState = scene.camera.getCameraState(time);
	Vector3 cameraPosition = cameraState.position;
	Vector3 cameraRight = cameraState.right;
	Vector3 cameraUp = cameraState.up;

	Color sampledPixelColor;
	uint64_t n = scene.general.cameraSamplesSqrt;

	for (uint64_t y = 0; y < n; ++y)
	{
		for (uint64_t x = 0; x < n; ++x)
		{
			Ray primaryRay;
			Vector2 jitter = (sampler->getSample2D(x, y, n, n, permutation, generator) - Vector2(0.5, 0.5)) * 2.0;
			isValidRay = scene.camera.getRay(pixelCoordinate + jitter, primaryRay, time);

			if (!isValidRay)
			{
				sampledPixelColor += scene.general.offLensColor;
				continue;
			}

			Vector3 focalPoint = primaryRay.origin + primaryRay.direction * focalDistance;
			Vector2 discCoordinate = sampler->getDiscSample(x, y, n, n, permutation, generator);

			Ray sampleRay;
			sampleRay.origin = cameraPosition + ((discCoordinate.x * apertureSize) * cameraRight + (discCoordinate.y * apertureSize) * cameraUp);
			sampleRay.direction = (focalPoint - sampleRay.origin).normalized();
			sampleRay.time = time;
			sampleRay.precalculate();

			sampledPixelColor += trace(scene, sampleRay, generator, interrupted);
		}
	}

	return sampledPixelColor / (double(n) * double(n));
}
