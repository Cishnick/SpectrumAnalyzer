#pragma once
#include "ISpectrumAnalyser.h"

using f_type = fd_t;

class SpectrumAnalyzer : public ISpectrumAnalyser
{
public:
	SpectrumAnalyzer(UINT32 SampleFrenq);

	void add(IObserverSpectr *obs) override;

	void remove(IObserverSpectr *obs) override;

	void Initialize(size_t n, ChannelsMode c_mode, fd_t func, GraphMode g_mode) override;

	Reals FFTW(Reals const &data, fd_t func, GraphMode mode) override;

	double FrenqStep() override;

	void ChangePacketSize(size_t new_n) override;

	void Release() override;

	void Buffer_OVF(vector<double> const &, vector<double> const &) override;

	void SetChannelsMode(ChannelsMode mode) override;

	void SetWindowFunc(fd_t func) override;

	// Вызывается для отправки данных через IObserverGraph
	void Handle(Reals &, Reals &, ChannelsMode, GraphMode);

private:
	// Выделяет память под массивы для FFT
	void CreateAB(size_t n);

	// Освобождает память, выделенную под массивы для FFT
	void DeleteAB(size_t n);

	// Выполняет оконную функцию с каждым элементов данных
	void WindowF(w_type *data, size_t size, fd_t func);

	// Фурье-преобразование для массива комплексных чисел
	void fft0(w_type *data, size_t n);

	// Считает целую часть log2(n). Очень быстро.
	size_t Log2N(size_t n);

	// Суммирует данные, нужно для FFT
	void w_mac(w_type *cc, w_type a, w_type w, w_type b);

	double _FrenqStep;
	double _SampleFrenq;
	w_type **_A;
	w_type **_B;
	size_t _N;
	ChannelsMode _CMode;
	GraphMode _GMode;
	f_type _WindowF;
	std::list<IObserverSpectr *> _Observers;
};
