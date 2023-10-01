#pragma once

#include "stdafx.h"

#include <gdiplus.h>

#include "GraphBox.h"
#include "IGUI.h"
#include "SettDialog.h"

class GUI : public IGUI
{
public:
	GUI(size_t t_size, double f_step, int width, int heght);

	int Initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine) override;

	void CreateHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd) override;

	void Rendering(HWND hWnd) override;

	void DrawSpectr(Reals &R, Reals &L, ChannelsMode cmode, GraphMode gmode) override;

	void Release() override;

	void ChangeSize(int width, int height) override;

	void CommandHandler(WPARAM wParam, LPARAM lParam) override;

	void setDevList(VectorOfString const &devs) override;

	void connectCom(IObserverCommand *obj) override;

	void SetPacketSize(int n, double f_step) override;

	void CreatePaintArea(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd);

	void add(IObserverCommand &obs);

	void remove(IObserverCommand &obs);

	void Switch(bool state);

	void ChangeDev(int index);

private:
	void OnPaint(HDC hdc);

	Gdiplus::Rect _PaintRect;
	HWND _HWnd;
	ULONG_PTR _GdiToken;
	Gdiplus::Point *_GraphPoints;
	size_t _NOfPoints;
	Gdiplus::Rect _ClientRect;
	GraphBox _Graphic;
	std::list<IObserverCommand *> _observers;
	RECT _PaintRECT;

	double _MaxFrenq;
	double _MaxAmpl;

	HWND _HComboSelectDev;

	HWND _HEditXMin;
	HWND _HEditXMax;
	HWND _HEditXSplit;

	HWND _HEditYMin;
	HWND _HEditYMax;
	HWND _HEditYSplit;

	HWND _HCheckXLog;
	HWND _HCheckYLog;

	HWND _HLabelYMin;
	HWND _HLabelYMax;

	SettDialog _SDialog;
};
