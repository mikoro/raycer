// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#include "Raytracing/Tracers/Tracer.h"

namespace Raycer
{
	class Color;
	class Scene;
	class Vector3;
	class Ray;
	struct Intersection;
	struct Light;
	struct DirectionalLight;
	struct PointLight;

	class Raytracer : public Tracer
	{
	protected:

		Color trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted) override;

	private:

		Color traceRecursive(const Scene& scene, const Ray& ray, Intersection& intersection, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted);

		void calculateNormalMapping(Intersection& intersection);
		void calculateRayReflectanceAndTransmittance(const Ray& ray, const Intersection& intersection, double& rayReflectance, double& rayTransmittance);
		Color calculateReflectedColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double rayReflectance, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted);
		Color calculateTransmittedColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double rayTransmittance, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted);
		Color calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusionAmount, std::mt19937& generator);
		Color calculatePhongShadingColor(const Vector3& normal, const Vector3& directionToLight, const Vector3& directionToCamera, const Light& light, const Color& diffuseReflectance, const Color& specularReflectance, double shininess);
		Color calculateSimpleFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor);
		double calculateAmbientOcclusionAmount(const Scene& scene, const Intersection& intersection, std::mt19937& generator);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const DirectionalLight& light);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const PointLight& light, std::mt19937& generator);
	};
}
