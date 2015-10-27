// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#include "Raytracing/Tracers/Tracer.h"

namespace Raycer
{
	struct TracerState;
	class Color;
	class Ray;
	
	class PathTracer : public Tracer
	{
	protected:

		Color trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted) override;

	private:

		Color traceRecursive(const Scene& scene, const Ray& ray, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted);
	};
}
