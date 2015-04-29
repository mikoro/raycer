// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	class Texture
	{
	public:

		virtual void initialize() = 0;

		virtual Color getColor(const Vector3& position, const Vector2& texcoord) const = 0;
		virtual double getValue(const Vector3& position, const Vector2& texcoord) const = 0;

		int id = 0;
		double intensity = 1.0;
	};
}