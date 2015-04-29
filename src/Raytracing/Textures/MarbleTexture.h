// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Utils/PerlinNoise.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class MarbleTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		Color marbleColor;
		Color streakColor;

	private:

		PerlinNoise perlinNoise;
	};
}
