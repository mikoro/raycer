// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	struct RaytracerState;
	struct Ray;

	class Raytracer
	{
	public:

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		void traceRay(RaytracerState& state, Ray& ray, int& rayCount, std::atomic<bool>& interrupted);
	};
}
