#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

struct WF
{
	static double Sinus(size_t k, size_t N) { return sin(M_PI * k / N); }

	static double Hann(size_t k, size_t N) { return 0.5 - 0.5 * cos(2 * M_PI * k / N); }

	static double BartlettHann(size_t k, size_t N) { return 0.62 - 0.48 * fabs(k / N - 0.5) - 0.38 * cos(2 * M_PI * k / N); }

	static double Hamming(size_t k, size_t N) { return 0.42 - 0.50 * cos(2 * M_PI * k / N) + 0.08 * cos(4 * M_PI * k / N); }

	static double Blackman(size_t k, size_t N) { return 0.42 - 0.50 * cos(2 * M_PI * k / N) + 0.08 * cos(4 * M_PI * k / N); }

	static double BlackmanHarris(size_t k, size_t N)
	{
		return 0.35875 - 0.48829 * cos(2 * M_PI * k / N) + 0.14128 * cos(4 * M_PI * k / N) + 0.01168 * cos(6 * M_PI * k / N);
	}

	static double Nuttall(size_t k, size_t N)
	{
		return 0.355768 - 0.487396 * cos(2 * M_PI * k / N) + 0.144232 * cos(4 * M_PI * k / N)
		     + 0.012604 * cos(6 * M_PI * k / N);
	}

	static double BlackmanNuttall(size_t k, size_t N)
	{
		return 0.3635819 - 0.4891775 * cos(2 * M_PI * k / N) + 0.1365995 * cos(4 * M_PI * k / N)
		     + 0.0106411 * cos(6 * M_PI * k / N);
	}

	static double Gaussian(size_t k, size_t N, double s)
	{
		double A = N / 2;
		return exp(-0.5 * pow((k - A / s * A), 2));
	}
};
