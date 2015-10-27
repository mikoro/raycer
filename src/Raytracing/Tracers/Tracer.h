// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <random>

#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"

namespace Raycer
{
	struct TracerState;
	class Scene;
	class Film;
	class Ray;
	class Color;
	class Vector2;

	enum class TracerType { RAY, PATH };

	class Tracer
	{
	public:

		Tracer();
		virtual ~Tracer() {}

		void run(TracerState& state, std::atomic<bool>& interrupted);

		static std::unique_ptr<Tracer> getTracer(TracerType type);

	protected:

		virtual Color trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted) = 0;

		std::map<SamplerType, std::unique_ptr<Sampler>> samplers;
		std::map<FilterType, std::unique_ptr<Filter>> filters;

	private:

		void generateMultiSamples(const Scene& scene, Film& film, const Vector2& pixelCoordinate, uint64_t pixelIndex, std::mt19937& generator, const std::atomic<bool>& interrupted);
		Color generateTimeSamples(const Scene& scene, const Vector2& pixelCoordinate, std::mt19937& generator, const std::atomic<bool>& interrupted);
		Color generateCameraSamples(const Scene& scene, const Vector2& pixelCoordinate, double time, std::mt19937& generator, const std::atomic<bool>& interrupted);
		
		std::vector<std::mt19937> generators;
	};
}
