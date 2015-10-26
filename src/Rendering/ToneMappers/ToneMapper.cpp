// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/ToneMapper.h"
#include "Rendering/ToneMappers/PassthroughToneMapper.h"
#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Rendering/ToneMappers/SimpleToneMapper.h"
#include "Rendering/ToneMappers/ReinhardToneMapper.h"

using namespace Raycer;

std::unique_ptr<ToneMapper> ToneMapper::getToneMapper(ToneMapperType type)
{
	switch (type)
	{
		case ToneMapperType::PASSTHROUGH: return std::make_unique<PassthroughToneMapper>();
		case ToneMapperType::LINEAR: return std::make_unique<LinearToneMapper>();
		case ToneMapperType::SIMPLE: return std::make_unique<SimpleToneMapper>();
		case ToneMapperType::REINHARD: return std::make_unique<ReinhardToneMapper>();
		default: throw new std::runtime_error("Unknown tone mapper type");
	}
}
