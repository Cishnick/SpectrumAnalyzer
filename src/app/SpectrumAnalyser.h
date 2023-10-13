#pragma once
#include "ISpectrumAnalyser.h"

using f_type = WindowFunctor;

class SpectrumAnalyzer : public ISpectrumAnalyser
{
public:
	explicit SpectrumAnalyzer(UINT32 sampleFreq);

	void initialize(size_t n, WindowFunctor func) override;

	double freqStep() override;

	void changePacketSize(size_t new_n) override;

	std::vector<double> calculate(std::span<double> data) override;

	void setWindowFunc(WindowFunctor func) override;

private:
	double freqStep_;
	double sampleFreq_;
	f_type windowFunction_;
};
