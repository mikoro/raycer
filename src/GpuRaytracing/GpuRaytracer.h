// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	struct RaytraceInfo;

	class GpuRaytracer
	{
	public:

		void trace(RaytraceInfo& info, std::atomic<bool>& interrupted);
	};
}
