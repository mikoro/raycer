// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <complex>
#include <functional>
#include <vector>

namespace Raycer
{
	class Polynomial
	{
	public:

		Polynomial();
		Polynomial(int size);
		Polynomial(double* coefficients, int size);

		void setSize(int size);
		void setCoefficient(int index, double value);
		void setCoefficients(double* coefficients, int size);

		double evaluate(double x) const;
		std::complex<double> evaluate(const std::complex<double>& x) const;

		bool isNormalized() const;
		void normalize();

		std::vector<std::complex<double>> findAllRoots(int iterations = 64);
		std::vector<double> findAllPositiveRealRoots(int iterations = 64, double maxImagValue = 0.0001);
		bool findSmallestPositiveRealRoot(double& result, int iterations = 64, double maxImagValue = 0.0001);

	private:

		std::vector<double> coefficients;
		std::vector<std::complex<double>> seedRoots;
		std::vector<std::complex<double>> roots;
		std::vector<std::complex<double>> previousRoots;
		std::vector<double> positiveRealRoots;

		int size = 0;
		int degree = 0;
	};
}
