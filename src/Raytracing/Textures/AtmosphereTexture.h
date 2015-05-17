// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/ColorGradient.h"
#include "Math/EulerAngle.h"
#include "Utils/PerlinNoise.h"

namespace Raycer
{
	class AtmosphereTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		int seed = 1;
		Color topColor1 = Color(0.0, 0.1, 0.6);
		Color topColor2 = Color(0.0, 0.2, 0.7);
		Color middleColor = Color(0.0, 0.4, 0.9);
		Color horizonColor = Color(0.3, 0.6, 0.9);
		Color cloudColor = Color(0.5, 0.5, 0.5);
		Color sunColor = Color(1.0, 1.0, 1.0);
		EulerAngle sunOrientation = EulerAngle(0.0, 0.0, 0.0);
		double sunSize = 1000.0;
		bool hasClouds = true;
		bool hasSun = true;

	private:

		ColorGradient gradient;
		PerlinNoise noise;
		Vector3 sunDirection;
	};
}
