// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#include "Math/Color.h"
#include "Math/Vector3.h"

namespace Raycer
{
	struct RaytracerState;
	class Scene;
	struct Ray;

	struct TraceResult
	{
		Color pixelColor;
		Vector3 pixelPosition;
		double pixelDistance = 0.0;
	};

	class Raytracer
	{
	public:

		Raytracer();

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		TraceResult shootRays(Scene& scene, double x, double y, int& rayCount, std::atomic<bool>& interrupted);
		void traceRay(Scene& scene, Ray& ray, int& rayCount, std::atomic<bool>& interrupted);
		Color calculateLighting(Scene& scene, Ray& ray, int& rayCount, std::atomic<bool>& interrupted);

		std::mt19937 mt;
		std::uniform_real_distribution<double> random;
	};
}
