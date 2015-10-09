// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <random>

#include "Raytracing/Tracers/Tracer.h"
#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"
#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	struct TracerState;
	
	class PathTracer : public Tracer
	{
	public:

		PathTracer();

		void run(TracerState& state, std::atomic<bool>& interrupted) override;

	private:

		std::map<SamplerType, std::unique_ptr<Sampler>> samplers;
		std::map<FilterType, std::unique_ptr<Filter>> filters;
		std::map<ToneMapperType, std::unique_ptr<ToneMapper>> toneMappers;

		std::mt19937 generator;
		std::uniform_int_distribution<uint> randomPermutation;
	};
}
