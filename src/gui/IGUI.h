#pragma once
#include "src/app/stdafx.h"

#include <span>

#include "src/app/IObserver.h"

class IGUI
{
public:
	virtual ~IGUI() = default;

	virtual int initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine) = 0;

	virtual void createHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd) = 0;

	virtual void rendering(HWND hWnd) = 0;

	virtual void changeSize(int width, int height) = 0;

	virtual void commandHandler(WPARAM wParam, LPARAM lParam) = 0;

	virtual void drawSpectr(std::span<double> data) = 0;

	virtual void add(IObserverCommand &obs) = 0;

	virtual void remove(IObserverCommand &obs) = 0;

	virtual void setDevList(VectorOfString const &names) = 0;

	virtual void connectCom(IObserverCommand *obj) = 0;

	virtual void setPacketSize(int n, double f_step) = 0;
};

class GUIFactory
{
public:
	static IGUI *make(size_t t_size, double f_step, int width, int heght);
};
