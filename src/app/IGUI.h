#pragma once
#include "stdafx.h"

#include "IObserver.h"

class IGUI : public IObserverSpectr
{
public:
	virtual int Initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine) = 0;

	virtual void CreateHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd) = 0;

	virtual void Rendering(HWND hWnd) = 0;

	virtual void Release() = 0;

	virtual void ChangeSize(int width, int height) = 0;

	virtual void CommandHandler(WPARAM wParam, LPARAM lParam) = 0;

	virtual void add(IObserverCommand &obs) = 0;

	virtual void remove(IObserverCommand &obs) = 0;

	virtual void setDevList(VectorOfString const &names) = 0;

	virtual void connectCom(IObserverCommand *obj) = 0;

	virtual void SetPacketSize(int n, double f_step) = 0;
};

class GUIFactory
{
public:
	static IGUI *make(size_t t_size, double f_step, int width, int heght);
};
