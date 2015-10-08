// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/PathTracer.h"

using namespace Raycer;

PathTracer::PathTracer()
{
}

void PathTracer::run(TracerState& state, std::atomic<bool>& interrupted)
{
	(void)state;
	(void)interrupted;
}
