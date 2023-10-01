#pragma once
/*
	ISpectrumAnalyser - интерфей, предоставляющий доступ к функциям спектрального анализа сигнала.
*/
#include <complex>
#include <functional>

#include "IObserver.h"
#include "WindowFunc.h"

using w_type = std::complex<double>;

class ISpectrumAnalyser : public IObserverAudio
{
public:
	// Добавляет наблюдателя
	virtual void add(IObserverSpectr *obs) = 0;

	// Удаляет наблюдателя
	virtual void remove(IObserverSpectr *obs) = 0;

	// Инициализирует модуль с указанным размером пакета данных
	virtual void Initialize(size_t n, ChannelsMode c_mode, fd_t func, GraphMode g_mode) = 0;

	// Выполняет оконное фурье преобразование
	virtual Reals FFTW(Reals const &data, fd_t func, GraphMode mode) = 0;

	// Возвращает шаг по частоте между двумя соседними отсчетами, в Гц
	virtual double FrenqStep() = 0;

	// Изменяет размер пакета данных на ходу
	virtual void ChangePacketSize(size_t new_n) = 0;

	// Освобождает все ресурсы занятые классом
	virtual void Release() = 0;

	// Устанавливает режим работы относительно каналов
	virtual void SetChannelsMode(ChannelsMode mode) = 0;

	// Устанавливает оконную функцию
	virtual void SetWindowFunc(fd_t func) = 0;
};

struct FactoryAnalyzer
{
	static ISpectrumAnalyser *make(UINT32 SampleFrenq);
};
