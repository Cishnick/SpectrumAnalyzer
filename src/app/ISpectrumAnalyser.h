#pragma once
/*
	ISpectrumAnalyser - интерфей, предоставляющий доступ к функциям спектрального анализа сигнала.
*/
#include <complex>
#include <functional>
#include <span>

#include "IObserver.h"
#include "WindowFunc.h"

using complex_t = std::complex<double>;

class ISpectrumAnalyser
{
public:
	virtual void initialize(size_t n, WindowFunctor func) = 0;

	virtual double freqStep() = 0;

	virtual void changePacketSize(size_t new_n) = 0;

	virtual void setWindowFunc(WindowFunctor func) = 0;

	virtual std::vector<double> calculate(std::span<double> data) = 0;
};

struct FactoryAnalyzer
{
	static std::unique_ptr<ISpectrumAnalyser> make(UINT32 sampleFreq);
};
