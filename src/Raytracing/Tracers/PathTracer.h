// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "Raytracing/Tracers/Tracer.h"

namespace Raycer
{
	struct TracerState;
	
	class PathTracer : public Tracer
	{
	public:

		PathTracer();

		void run(TracerState& state, std::atomic<bool>& interrupted) override;

	private:

	};
}
