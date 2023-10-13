#pragma once

#include "src/app/stdafx.h"

#include <gdiplus.h>

#include "GraphBox.h"
#include "IGUI.h"
#include "SettDialog.h"

class GUI : public IGUI
{
public:
	GUI(size_t samplesNumber, double freqStep, int width, int heght);

	~GUI() override;

	int initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine) override;

	void createHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd) override;

	void rendering(HWND hWnd) override;

	void drawSpectr(std::span<double> data) override;

	void changeSize(int width, int height) override;

	void commandHandler(WPARAM wParam, LPARAM lParam) override;

	void setDevList(VectorOfString const &devs) override;

	void connectCom(IObserverCommand *obj) override;

	void setPacketSize(int n, double f_step) override;

	void createPaintArea(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd);

	void add(IObserverCommand &obs) override;

	void remove(IObserverCommand &obs) override;

	void Switch(bool state);

	void changeDevice(int index);

private:
	void OnPaint(HDC hdc);

	void invalidatePaintRect();

	Gdiplus::Rect paintRect;
	HWND hwnd;
	ULONG_PTR gdiToken;
	Gdiplus::Point *graphPoints;
	Gdiplus::Rect clientRect;
	GraphBox graphBox;
	std::list<IObserverCommand *> _observers;

	double maxFreq;
	double maxAmpl;

	HWND hComboSelectDev;

	HWND hEditXMin;
	HWND hEditXMax;
	HWND hEditXSplit;

	HWND hEditYMin;
	HWND hEditYMax;
	HWND hEditYSplit;

	HWND hCheckXLog;
	HWND hCheckYLog;

	HWND hLabelYMin;
	HWND hLabelYMax;

	SettDialog settingsDialog;
};
