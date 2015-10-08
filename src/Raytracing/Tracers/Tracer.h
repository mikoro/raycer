// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <memory>

namespace Raycer
{
	struct TracerState;

	enum class TracerType { WHITTED, PATH };

	class Tracer
	{
	public:

		virtual ~Tracer() {}

		virtual void run(TracerState& state, std::atomic<bool>& interrupted) = 0;

		static std::unique_ptr<Tracer> getTracer(TracerType type);
	};
}
