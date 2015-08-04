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
	struct Ray;
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

		Color generatePixelSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateCameraSamples(const Scene& scene, const Vector2& sampledPixelCoordinate, const std::atomic<bool>& interrupted);
		void raytrace(const Scene& scene, Ray& ray, const std::atomic<bool>& interrupted);
		Color calculateLightColor(const Scene& scene, const Ray& ray);
		Color calculateFogColor(const Scene& scene, const Ray& ray);
		Color calculatePhongShading(const Vector3& N, const Vector3& L, const Vector3& V, const Light* light, const Material* material);
		bool isInShadow(const Scene& scene, const Vector3& P, const Vector3& L, double distanceToLight);

		Sampler sampler;

		std::mt19937 gen;
		std::uniform_int_distribution<int> intDist;
	};
}
