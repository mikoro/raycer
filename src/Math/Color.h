// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>
#include <random>

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Color
	{
	public:

		explicit Color(double r = 0.0, double g = 0.0, double b = 0.0, double a = 1.0);
		explicit Color(int r, int g, int b, int a = 255);

		friend Color operator+(const Color& c1, const Color& c2);
		friend Color operator-(const Color& c1, const Color& c2);
		friend Color operator*(const Color& c1, const Color& c2);
		friend Color operator*(const Color& c, double s);
		friend Color operator*(double s, const Color& c);
		friend Color operator/(const Color& c, double s);

		friend bool operator==(const Color& c1, const Color& c2);
		friend bool operator!=(const Color& c1, const Color& c2);

		Color& operator+=(const Color& c);
		Color& operator-=(const Color& c);
		Color& operator*=(double s);
		Color& operator/=(double s);

		uint32_t getRgbaValue() const;
		uint32_t getAbgrValue() const;
		double getLuminance() const;
		bool isTransparent() const;
		bool isZero() const;
		bool isClamped() const;
		void clamp();
		Color clamped() const;

		static Color fromRgbaValue(uint32_t rgba);
		static Color fromAbgrValue(uint32_t abgr);
		static Color lerp(const Color& start, const Color& end, double alpha);
		static Color alphaBlend(const Color& first, const Color& second);
		static Color pow(const Color& color, double power);
		static Color fastPow(const Color& color, double power);
		static Color random();
		static Color random(std::mt19937& generator);

		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color WHITE;
		static const Color BLACK;

		double r;
		double g;
		double b;
		double a;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(r),
				CEREAL_NVP(g),
				CEREAL_NVP(b),
				CEREAL_NVP(a));
		}
	};
}
