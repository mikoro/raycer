// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <algorithm>
#include <complex>

namespace Raycer
{
	template <int N>
	class Polynomial
	{
	public:

		Polynomial();
		Polynomial(const double* coefficients);

		void setCoefficients(const double* coefficients);

		std::complex<double> evaluate(const std::complex<double>& x) const;

		const std::complex<double>* findAllRoots(int maxIterations = 64, double changeThreshold = 0.0001);
		const double* findAllPositiveRealRoots(int& count, int maxIterations = 64, double changeThreshold = 0.0001, double imagZeroThreshold = 0.0001);
		bool findSmallestPositiveRealRoot(double& result, int maxIterations = 64, double changeThreshold = 0.0001, double imagZeroThreshold = 0.0001);

	private:

		Polynomial(const Polynomial& p);
		Polynomial& operator=(const Polynomial& p);

		const int size = N;
		const int degree = N - 1;

		double coefficients[N];
		std::complex<double> roots[N - 1];
		std::complex<double> previousRoots[N - 1];
		double positiveRealRoots[N - 1];
	};
}

#include "Math/Polynomial.inl"
