// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Tracers/Tracer.h"
#include "Raytracing/Tracers/WhittedTracer.h"
#include "Raytracing/Tracers/PathTracer.h"

using namespace Raycer;

std::unique_ptr<Tracer> Tracer::getTracer(TracerType type)
{
	switch (type)
	{
		case TracerType::WHITTED: return std::unique_ptr<Tracer>(new WhittedTracer());
		case TracerType::PATH: return std::unique_ptr<Tracer>(new PathTracer());
		default: throw std::runtime_error("Invalid tracer type");;
	}
}
