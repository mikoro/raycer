// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class ColorTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector2 get2DValue(const Vector2& texcoord, const Vector3& position) const;

		Color color;
	};
}
