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

uint32_t Color::getRgbaValue() const
{
	assert(r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0 && a >= 0.0 && a <= 1.0);

	int r_ = (int)(r * 255.0 + 0.5);
	int g_ = (int)(g * 255.0 + 0.5);
	int b_ = (int)(b * 255.0 + 0.5);
	int a_ = (int)(a * 255.0 + 0.5);

	return (r_ << 24 | g_ << 16 | b_ << 8 | a_);
}

uint32_t Color::getAbgrValue() const
{
	assert(r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0 && a >= 0.0 && a <= 1.0);

	int r_ = (int)(r * 255.0 + 0.5);
	int g_ = (int)(g * 255.0 + 0.5);
	int b_ = (int)(b * 255.0 + 0.5);
	int a_ = (int)(a * 255.0 + 0.5);

	return (a_ << 24 | b_ << 16 | g_ << 8 | r_);
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

Color Color::fromRgbaValue(uint32_t rgba)
{
	int r = (rgba >> 24);
	int g = (rgba >> 16) & 0xff;
	int b = (rgba >> 8) & 0xff;
	int a = rgba & 0xff;
	
	const double inv255 = 1.0 / 255.0;

	Color c;

	c.r = r * inv255;
	c.g = g * inv255;
	c.b = b * inv255;
	c.a = a * inv255;

	return c;
}

Color Color::fromAbgrValue(uint32_t abgr)
{
	int r = abgr & 0xff;
	int g = (abgr >> 8) & 0xff;
	int b = (abgr >> 16) & 0xff;
	int a = (abgr >> 24);

	const double inv255 = 1.0 / 255.0;

	Color c;

	c.r = r * inv255;
	c.g = g * inv255;
	c.b = b * inv255;
	c.a = a * inv255;

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
	const double alpha = second.a;
	const double invAlpha = 1.0 - alpha;

	Color c;

	c.r = (alpha * second.r + invAlpha * first.r);
	c.g = (alpha * second.g + invAlpha * first.g);
	c.b = (alpha * second.b + invAlpha * first.b);
	c.a = 1.0;

	return c;
}
