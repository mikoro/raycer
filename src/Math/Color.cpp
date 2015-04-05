// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>

#include "Math/Color.h"
#include "Math/MathUtils.h"

using namespace Raycer;

const Color Color::RED = Color(1.0, 0.0, 0.0);
const Color Color::GREEN = Color(0.0, 1.0, 0.0);
const Color Color::BLUE = Color(0.0, 0.0, 1.0);
const Color Color::WHITE = Color(1.0, 1.0, 1.0);
const Color Color::BLACK = Color(0.0, 0.0, 0.0);

Color::Color(double r_, double g_, double b_, double a_) : r(r_), g(g_), b(b_), a(a_)
{
}

Color::Color(int r_, int g_, int b_, int a_)
{
	assert(r_ >= 0 && r_ <= 255 && g_ >= 0 && g_ <= 255 && b_ >= 0 && b_ <= 255 && a_ >= 0 && a_ <= 255);

	double inv255 = 1.0 / 255.0;

	r = r_ * inv255;
	g = g_ * inv255;
	b = b_ * inv255;
	a = a_ * inv255;
}

Color::Color(uint32_t abgr)
{
	int r_ = abgr & 0xff;
	int g_ = (abgr >> 8) & 0xff;
	int b_ = (abgr >> 16) & 0xff;
	int a_ = (abgr >> 24);

	double inv255 = 1.0 / 255.0;

	r = r_ * inv255;
	g = g_ * inv255;
	b = b_ * inv255;
	a = a_ * inv255;
}

uint32_t Color::getAbgrValue() const
{
	assert(r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0 && a >= 0.0 && a <= 1.0);

	int ri = (int)(r * 255.0 + 0.5);
	int gi = (int)(g * 255.0 + 0.5);
	int bi = (int)(b * 255.0 + 0.5);
	int ai = (int)(a * 255.0 + 0.5);

	return (ai << 24 | bi << 16 | gi << 8 | ri);
}

Color Raycer::operator+(const Color& c1, const Color& c2)
{
	return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
}

Color Raycer::operator-(const Color& c1, const Color& c2)
{
	return Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
}

Color Raycer::operator*(const Color& c1, const Color& c2)
{
	return Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
}

Color Raycer::operator*(const Color& c, double s)
{
	return Color(c.r * s, c.g * s, c.b * s, c.a * s);
}

Color Raycer::operator*(double s, const Color& c)
{
	return Color(c.r * s, c.g * s, c.b * s, c.a * s);
}

Color Raycer::operator/(const Color& c, double s)
{
	return Color(c.r / s, c.g / s, c.b / s, c.a / s);
}

bool Raycer::operator==(const Color& c1, const Color& c2)
{
	return MathUtils::almostSame(c1.r, c2.r) && MathUtils::almostSame(c1.g, c2.g) && MathUtils::almostSame(c1.b, c2.b) && MathUtils::almostSame(c1.a, c2.a);
}

bool Raycer::operator!=(const Color& c1, const Color& c2)
{
	return !(c1 == c2);
}

Color& Color::operator+=(const Color& c)
{
	*this = *this + c;

	return *this;
}

Color& Color::operator-=(const Color& c)
{
	*this = *this - c;

	return *this;
}

Color& Color::operator*=(double s)
{
	*this = *this * s;

	return *this;
}

Color& Color::operator/=(double s)
{
	*this = *this / s;

	return *this;
}

bool Color::isTransparent() const
{
	return (a < 1.0);
}

void Color::clamp()
{
	*this = clamped();
}

Color Color::clamped() const
{
	Color c;

	c.r = std::max(0.0, std::min(r, 1.0));
	c.g = std::max(0.0, std::min(g, 1.0));
	c.b = std::max(0.0, std::min(b, 1.0));
	c.a = std::max(0.0, std::min(a, 1.0));

	return c;
}

Color Color::lerp(const Color& start, const Color& end, double alpha)
{
	Color c;

	c.r = start.r + (end.r - start.r) * alpha;
	c.g = start.g + (end.g - start.g) * alpha;
	c.b = start.b + (end.b - start.b) * alpha;
	c.a = start.a + (end.a - start.a) * alpha;

	return c;
}

Color Color::alphaBlend(const Color& first, const Color& second)
{
	double alpha = second.a;
	double invAlpha = 1.0 - alpha;

	Color c;

	c.r = (alpha * second.r + invAlpha * first.r);
	c.g = (alpha * second.g + invAlpha * first.g);
	c.b = (alpha * second.b + invAlpha * first.b);
	c.a = 1.0;

	return c;
}

uint32_t Color::alphaBlend(uint32_t first, uint32_t second)
{
	int r1 = first & 0xff;
	int g1 = (first >> 8) & 0xff;
	int b1 = (first >> 16) & 0xff;

	int r2 = second & 0xff;
	int g2 = (second >> 8) & 0xff;
	int b2 = (second >> 16) & 0xff;
	int alpha = (second >> 24) + 1;
	int invAlpha = 257 - alpha;

	int r3 = (alpha * r2 + invAlpha * r1) >> 8;
	int g3 = (alpha * g2 + invAlpha * g1) >> 8;
	int b3 = (alpha * b2 + invAlpha * b1) >> 8;

	return (0xff << 24 | b3 << 16 | g3 << 8 | r3);
}
