// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <CL/opencl.h>

#ifdef _MSC_VER
#define ALIGN(x) __declspec(align(x))
#elif __GNUC__
#define ALIGN(x) __attribute__ (aligned(x))
#endif

namespace Raycer
{
	namespace Gpu
	{
		struct Sphere
		{
			cl_float4 ALIGN(16) position;
			cl_float ALIGN(4) radius;
		};
	}
}
