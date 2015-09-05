// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <cmath>

#include "Math/Polynomial.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Polynomial::Polynomial()
{
}

Polynomial::Polynomial(int size_)
{
	setSize(size_);
}

Polynomial::Polynomial(double* coefficients_, int size_)
{
	setCoefficients(coefficients_, size_);
}

void Polynomial::setSize(int size_)
{
	size = size_;
	degree = size_ - 1;

	coefficients.resize(size);
	seedRoots.resize(degree);
	roots.resize(degree);
	previousRoots.resize(degree);
	positiveRealRoots.reserve(degree);

	for (int i = 0; i < size; ++i)
		coefficients[i] = 0.0;

	std::complex<double> seed(0.4, 0.9);

	for (int i = 0; i < degree; ++i)
		seedRoots[i] = pow(seed, i);

	roots = seedRoots;
	previousRoots = seedRoots;
}

void Polynomial::setCoefficient(int index, double value)
{
	coefficients[index] = value;
}

void Polynomial::setCoefficients(double* values, int size_)
{
	if (size_ != size)
		setSize(size_);

	for (int i = 0; i < size; ++i)
		coefficients[i] = values[i];
}

double Polynomial::evaluate(double x) const
{
	double y = 0.0;

	for (int i = 0; i < size; ++i)
		y += coefficients[i] * pow(x, degree - i);

	return y;
}

std::complex<double> Polynomial::evaluate(const std::complex<double>& x) const
{
	std::complex<double> y(0.0, 0.0);

	for (int i = 0; i < size; ++i)
		y += coefficients[i] * pow(x, degree - i);

	return y;
}

bool Polynomial::isNormalized() const
{
	return coefficients[0] == 1.0;
}

void Polynomial::normalize()
{
	double denominator = coefficients[0];

	for (int i = 0; i < size; ++i)
		coefficients[i] /= denominator;
}

// Durand-Kerner algorithm
// https://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
std::vector<std::complex<double>> Polynomial::findAllRoots(int iterations)
{
	if (!isNormalized())
		normalize();

	roots = seedRoots;
	previousRoots = seedRoots;

	for (int i = 0; i < iterations; ++i)
	{
		for (int j = 0; j < degree; ++j)
		{
			// numerator
			std::complex<double> temp = evaluate(roots[j]);

			// denominators
			for (int k = 0; k < degree; ++k)
			{
				if (k == j)
					continue;

				temp /= (roots[j] - roots[k]);
			}

			// rest of the equation
			roots[j] = roots[j] - temp;
		}

		// break iteration if roots are not changing much
		if (i >= 4)
		{
			bool notChanged = true;

			for (int j = 0; j < degree; ++j)
			{
				if (!MathUtils::almostSame(roots[j], previousRoots[j]))
				{
					notChanged = false;
					break;
				}
			}

			if (notChanged)
				break;
		}

		for (int j = 0; j < degree; ++j)
			previousRoots[j] = roots[j];
	}

	return roots;
}

std::vector<double> Polynomial::findAllPositiveRealRoots(int iterations, double maxImagValue)
{
	findAllRoots(iterations);
	positiveRealRoots.clear();

	for (int i = 0; i < degree; ++i)
	{
		if (std::abs(roots[i].imag()) < maxImagValue && roots[i].real() > 0.0)
			positiveRealRoots.push_back(roots[i].real());
	}

	return positiveRealRoots;
}

bool Polynomial::findSmallestPositiveRealRoot(double& result, int iterations, double maxImagValue)
{
	findAllPositiveRealRoots(iterations, maxImagValue);

	if (positiveRealRoots.size() == 0)
		return false;

	std::sort(positiveRealRoots.begin(), positiveRealRoots.end());
	result = positiveRealRoots[0];

	return true;
}
