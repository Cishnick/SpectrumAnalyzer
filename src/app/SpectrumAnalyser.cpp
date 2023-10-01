#include "stdafx.h"

#include "SpectrumAnalyser.h"

// -----------------------------------------------------Конструктор--------------------------

SpectrumAnalyzer::SpectrumAnalyzer(UINT32 SampleFrenq)
	: _SampleFrenq(SampleFrenq)
	, _FrenqStep(0)
	, _A(nullptr)
	, _B(nullptr)
	, _N(0)
	, _CMode(ChannelsMode::Right)
	, _GMode(GraphMode::Amplitude)
{
}

// -------------------------------------------------------------add--------------------------

void SpectrumAnalyzer::add(IObserverSpectr *obs)
{
	_Observers.push_back(obs);
}

// ------------------------------------------------------remove------------------------------

void SpectrumAnalyzer::remove(IObserverSpectr *obs)
{
	_Observers.remove(obs);
}

// ---------------------------------------------------initialize-----------------------------

void SpectrumAnalyzer::Initialize(size_t n, ChannelsMode c_mode, fd_t func, GraphMode g_mode)
{
	_CMode = c_mode;
	_GMode = g_mode;
	_WindowF = func;
	size_t ln = Log2N(n) + 1;
	_N = 1ULL << ln;
	CreateAB(_N);
	_FrenqStep = _SampleFrenq / _N;
}

// ----------------------------------------------------FFTW----------------------------------

Reals SpectrumAnalyzer::FFTW(Reals const &data, fd_t func, GraphMode mode)
{
	size_t size = data.size();
	size_t ln = Log2N(size) + 1;
	size_t N = (1ULL << ln);
	double r;
	w_type *tmp = new w_type[1 << ln];
	for (size_t i = 0; i < N; i++)
	{
		r = (i < size) ? data[i] : 0;
		tmp[i] = w_type(r, 0);
	}
	WindowF(tmp, N, func);
	fft0(tmp, N);
	Reals res(N);
	if (mode == Amplitude)
		for (size_t i = 0; i < N; i++)
		{
			res[i] = std::abs(tmp[i]) / N;
		}
	else
		for (size_t i = 0; i < N; i++)
		{
			res[i] = std::arg(tmp[i]) / N;
		}
	return std::move(res);
}

// ----------------------------------------------FrenqStep-----------------------------------

double SpectrumAnalyzer::FrenqStep()
{
	return _FrenqStep;
}

// -------------------------------------------ChangePacketSize------------------------------

void SpectrumAnalyzer::ChangePacketSize(size_t new_n)
{
	Release();
	Initialize(new_n, _CMode, _WindowF, _GMode);
}

// -------------------------------------------------Release---------------------------------

void SpectrumAnalyzer::Release()
{
	DeleteAB(_N);
}

// -------------------------------------------Buffer_OVF------------------------------------

void SpectrumAnalyzer::Buffer_OVF(vector<double> const &R, vector<double> const &L)
{
	Reals res_R;
	Reals res_L;
	switch (_CMode)
	{
		case ChannelsMode::Right: res_R = FFTW(R, _WindowF, _GMode); break;
		case ChannelsMode::Left: res_R = FFTW(L, _WindowF, _GMode); break;
		case ChannelsMode::RightPlusLeft:
		{
			Reals temp = R;
			for (size_t i = 0; i < R.size(); i++)
			{
				temp[i] += L[i];
			}
			res_R = FFTW(temp, _WindowF, _GMode);
		}
		break;
		case ChannelsMode::RightMinusLeft:
		{
			Reals temp = R;
			for (size_t i = 0; i < R.size(); i++)
			{
				temp[i] -= L[i];
			}
			res_R = FFTW(temp, _WindowF, _GMode);
		}
		break;
		case ChannelsMode::RightAndLeft:
			res_R = FFTW(R, _WindowF, _GMode);
			res_L = FFTW(L, _WindowF, _GMode);
			break;
	}
	Handle(res_R, res_L, _CMode, _GMode);
}

// ---------------------------------------SetChannelsMode-----------------------------------

void SpectrumAnalyzer::SetChannelsMode(ChannelsMode mode)
{
	_CMode = mode;
}

// ------------------------------------------------SetWindowFunc----------------------------

void SpectrumAnalyzer::SetWindowFunc(fd_t func)
{
	_WindowF = func;
}

// ---------------------------------------Handle--------------------------------------------

void SpectrumAnalyzer::Handle(Reals &R, Reals &L, ChannelsMode cm, GraphMode gm)
{
	for (auto i : _Observers)
	{
		i->DrawSpectr(R, L, cm, gm);
	}
}

// ---------------------------------------------CreateAB------------------------------------

void SpectrumAnalyzer::CreateAB(size_t n)
{
	size_t ln = Log2N(n);
	size_t k = 2;
	_A = new w_type *[ln];
	_B = new w_type *[ln];
	for (size_t i = 0; i < ln; i++, k *= 2)
	{
		_A[i] = new w_type[n / k];
		_B[i] = new w_type[n / k];
	}
}

// ----------------------------------------------------DeleteAB------------------------------

void SpectrumAnalyzer::DeleteAB(size_t n)
{
	size_t ln = Log2N(n);
	for (size_t i = 0; i < ln; i++)
	{
		delete[] _A[i];
		delete[] _B[i];
	}
	delete[] _A;
	delete[] _B;
}

// ----------------------------------------WindowF------------------------------------------

void SpectrumAnalyzer::WindowF(w_type *data, size_t size, fd_t func)
{
	for (size_t i = 0; i < size; i++)
		data[i] *= func(i, size - 1);
}

// ----------------------------------------fft0---------------------------------------------

void SpectrumAnalyzer::fft0(w_type *data, size_t n)
{
	int i;
	w_type w;
	double temp;
	int rec = Log2N(_N / n);
	if (n == 1)
	{
		return;
	}
	for (i = 0; i < n / 2; i++)
	{
		_A[rec][i] = data[i * 2];
		_B[rec][i] = data[i * 2 + 1];
	}
	fft0(_A[rec], n / 2);
	fft0(_B[rec], n / 2);
	for (i = 0; i < n; i++)
	{
		temp = 2 * M_PI * i / n;
		w = w_type(cos(temp), -sin(temp));
		w_mac(data + i, _A[rec][i % (n / 2)], w, _B[rec][i % (n / 2)]);
	}
}

// --------------------------------------------Log2N-----------------------------------------

size_t SpectrumAnalyzer::Log2N(size_t n)
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

// --------------------------------------------w_mac-----------------------------------------

void SpectrumAnalyzer::w_mac(w_type *cc, w_type a, w_type w, w_type b)
{
	double r = a.real() + w.real() * b.real() - w.imag() * b.imag();
	double i = a.imag() + w.real() * b.imag() + w.imag() * b.real();
	*cc = w_type(r, i);
}

// -------------------------------------------------Factory::make----------------------------

ISpectrumAnalyser *FactoryAnalyzer::make(UINT32 SampleFrenq)
{
	return new SpectrumAnalyzer(SampleFrenq);
}
