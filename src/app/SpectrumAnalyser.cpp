#include "stdafx.h"

#include "SpectrumAnalyser.h"

namespace
{
int log2N(size_t n)
{
	return n / 256 ? n / 4048 ? n / 16348 ? n / 32768 ? 15 : 14
	                          : n / 8096  ? 13
	                                      : 12
	               : n / 1024 ? n / 2048 ? 11 : 10
	               : n / 512  ? 9
	                          : 8
	     : n / 16  ? n / 64 ? n / 128 ? 7 : 6
	               : n / 32 ? 5
	                        : 4
	     : n / 4   ? n / 8 ? 3 : 2
	     : n / 2   ? 1
	               : 0;
}

void applyWindowFunction(std::vector<complex_t> &data, size_t size, const WindowFunctor &func)
{
	for (size_t i = 0; i < size; i++)
		data[i] *= func(i, size - 1);
}

void w_mac(complex_t &cc, complex_t a, complex_t w, complex_t b)
{
	double real = a.real() + w.real() * b.real() - w.imag() * b.imag();
	double imag = a.imag() + w.real() * b.imag() + w.imag() * b.real();
	cc = complex_t(real, imag);
}

struct FFT
{
	explicit FFT(unsigned size)
		: size(size)
	{
		size_t ln = log2N(size);
		size_t k = 2;
		evenSamples = new complex_t *[ln];
		unevenSamples = new complex_t *[ln];
		for (size_t i = 0; i < ln; i++, k *= 2)
		{
			evenSamples[i] = new complex_t[size / k];
			unevenSamples[i] = new complex_t[size / k];
		}
	}

	~FFT()
	{
		size_t ln = log2N(size);
		for (size_t i = 0; i < ln; i++)
		{
			delete[] evenSamples[i];
			delete[] unevenSamples[i];
		}
		delete[] evenSamples;
		delete[] unevenSamples;
	}

	void operator()(std::span<complex_t> data)
	{
		unsigned n = data.size();
		if (n == 1)
			return;

		int rec = log2N(size / n);
		for (auto i = 0; i < n / 2; i++)
		{
			evenSamples[rec][i] = data[i * 2];
			unevenSamples[rec][i] = data[i * 2 + 1];
		}
		operator()(std::span<complex_t>(evenSamples[rec], n / 2));
		operator()(std::span<complex_t>(unevenSamples[rec], n / 2));
		for (auto i = 0; i < n; i++)
		{
			auto temp = 2 * M_PI * i / n;
			auto w = complex_t(cos(temp), -sin(temp));
			w_mac(data[i], evenSamples[rec][i % (n / 2)], w, unevenSamples[rec][i % (n / 2)]);
		}
	}

private:
	complex_t **evenSamples;
	complex_t **unevenSamples;
	unsigned size;
};

std::vector<complex_t> makeComplexFromReal(std::span<double> data)
{
	size_t ln = log2N(data.size()) + 1;
	unsigned N = (1ULL << ln);

	std::vector<complex_t> complexData(N);
	for (size_t i = 0; i < N; i++)
	{
		auto r = (i < data.size()) ? data[i] : 0;
		complexData[i] = complex_t(r, 0);
	}
	return complexData;
}

std::vector<double> calcWindowFFT(std::span<double> data, const WindowFunctor &func)
{
	unsigned N = (1 << (log2N(data.size()) + 1));

	auto complexData = makeComplexFromReal(data);
	applyWindowFunction(complexData, N, func);
	FFT{N}(complexData);

	Reals res;
	std::ranges::transform(complexData, std::back_inserter(res), [N](const complex_t &value) { return std::abs(value) / N; });
	return res;
}

} // namespace

SpectrumAnalyzer::SpectrumAnalyzer(UINT32 sampleFreq)
	: sampleFreq_(sampleFreq)
	, freqStep_(0)
{
}

void SpectrumAnalyzer::initialize(size_t n, WindowFunctor func)
{
	windowFunction_ = func;
	size_t ln = log2N(n) + 1;
	freqStep_ = sampleFreq_ / (1 << ln);
}

double SpectrumAnalyzer::freqStep()
{
	return freqStep_;
}

void SpectrumAnalyzer::changePacketSize(size_t new_n)
{
	initialize(new_n, windowFunction_);
}

std::vector<double> SpectrumAnalyzer::calculate(std::span<double> data)
{
	return calcWindowFFT(data, windowFunction_);
}

void SpectrumAnalyzer::setWindowFunc(WindowFunctor func)
{
	windowFunction_ = std::move(func);
}

std::unique_ptr<ISpectrumAnalyser> FactoryAnalyzer::make(UINT32 sampleFreq)
{
	return std::make_unique<SpectrumAnalyzer>(sampleFreq);
}
