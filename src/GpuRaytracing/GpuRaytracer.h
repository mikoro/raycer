// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	struct CpuRaytracerConfig;

	class GpuRaytracer
	{
	public:

		void trace(CpuRaytracerConfig& config, std::atomic<bool>& interrupted);
	};
}
