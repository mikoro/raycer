// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Vector3.h"

namespace Raycer
{
	class Matrix4x4;

	class AxisAngle
	{
	public:

		explicit AxisAngle(const Vector3& axis = Vector3::UP, double angle = 0.0);

		Matrix4x4 toMatrix4x4() const;

		Vector3 axis;
		double angle; // degrees

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(axis),
				CEREAL_NVP(angle));
		}
	};
}
