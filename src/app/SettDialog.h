#pragma once
#include "stdafx.h"

#include "IObserver.h"
#include "SettStructs.h"
#include "WindowFunc.h"
#include "resource.h"

class SettDialog
{
public:
	void connectGraph(IObserverGraph *obj);

	void connectCommand(IObserverCommand *obj);

	void Show(HWND hWnd);

	bool isWork();

	void Destroy();

private:
	HWND _hWnd;
	HWND _hTreeControl;

	bool _work = false;
};
