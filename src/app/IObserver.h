#pragma once
#include "stdafx.h"

using std::vector;

class IObserverCommand
{
public:
	virtual ~IObserverCommand() = default;

	virtual void switchCapturing(bool is_turnon) = 0;

	virtual void changeDevice(int index) = 0;

	virtual void setWindowFunction(WindowFunctor func) = 0;

	virtual void setSamples(int n_sample) = 0;
};

class IObserverGraph
{
public:
	virtual ~IObserverGraph() = default;

	virtual void setColors(const struct ColorsGraph &sett) = 0;
};
