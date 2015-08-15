// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#include "Utils/Sampler.h"

namespace Raycer
{
	class Color;
	class Scene;
	class Vector2;
	struct RaytracerState;
	class Ray;
	struct Intersection;
	struct Light;
	struct Material;
	
	class Raytracer
	{
	public:

		Raytracer();

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		Raytracer(const Raytracer& raytracer);
		Raytracer& operator=(const Raytracer& raytracer);

		Color generateMultiSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateDofSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateTimeSamples(const Scene& scene, Ray& ray, const std::atomic<bool>& interrupted);
		Color raytrace(const Scene& scene, const Ray& ray, Intersection& intersection, int iteration, const std::atomic<bool>& interrupted);
		double calculateAmbientOcclusion(const Scene& scene, const Intersection& intersection);
		Color calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusion);
		Color calculateFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor);

		Sampler sampler;

		std::mt19937 gen;
		std::uniform_int_distribution<int> intDist;
	};
}
