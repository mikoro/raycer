// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#include "Utils/Sampler.h"

namespace Raycer
{
	struct RaytracerState;
	class Color;
	class Scene;
	struct Ray;
	
	class Raytracer
	{
	public:

		Raytracer();

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		Raytracer(const Raytracer& raytracer);
		Raytracer& operator=(const Raytracer& raytracer);

		Color shootRays(const Scene& scene, const Vector2& pixel, int& rayCount, const std::atomic<bool>& interrupted);
		void traceRay(const Scene& scene, Ray& ray, int& rayCount, const std::atomic<bool>& interrupted);
		Color calculateLightColor(const Scene& scene, const Ray& ray);
		Color calculateFogColor(const Scene& scene, const Ray& ray);

		Sampler sampler;

		std::mt19937 gen;
		std::uniform_int_distribution<int> intDist;
	};
}
