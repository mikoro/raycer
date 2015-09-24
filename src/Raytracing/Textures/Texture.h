// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	enum class TextureNormalDataType { NONE, BUMP_MAP, NORMAL_MAP, GRADIENT };

	class Texture
	{
	public:

		virtual ~Texture() {}

		virtual void initialize() = 0;

		virtual Color getColor(const Vector2& texcoord, const Vector3& position) const = 0;
		virtual double getValue(const Vector2& texcoord, const Vector3& position) const = 0;
		virtual Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const = 0;

		int id = 0;
		double intensity = 1.0;

	private:

		friend class cereal::access;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(id),
				CEREAL_NVP(intensity));
		}
	};
}
