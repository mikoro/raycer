// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/CellNoiseTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void CellNoiseTexture::initialize()
{
}

Color CellNoiseTexture::getColor(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return Color();
}

double CellNoiseTexture::getValue(const Vector3& position, const Vector2& texcoord) const
{
	(void)position;
	(void)texcoord;

	return 1.0;
}
