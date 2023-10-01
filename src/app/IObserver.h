#pragma once
#include "stdafx.h"

#include "SettStructs.h"
using std::vector;

// Данный интерфейс используется для передачи данных с аудиопотока в класс, реализующий
// преобразование Фурье.
class IObserverAudio
{
public:
	virtual void Buffer_OVF(vector<double> const &, vector<double> const &) = 0;
};

// Данный класс испоьзуется для передачи данных о спектре сигнала в модуль для графической
// отрисовки графика
class IObserverSpectr
{
public:
	virtual void DrawSpectr(Reals &R, Reals &L, ChannelsMode cmode, GraphMode gmode) = 0;
};

class IObserverCommand
{
public:
	virtual void Switch(bool is_turnon) = 0;

	virtual void changeDevice(int index) = 0;

	virtual void setWindowFunction(fd_t func) = 0;

	virtual void setSamples(int n_sample) = 0;
};

class IObserverGraph
{
public:
	virtual void SetColors(ColorsGraph const &sett) = 0;
};
