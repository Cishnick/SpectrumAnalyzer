#pragma once
#include "src/app/stdafx.h"

#include "src/app/IObserver.h"
#include "SettStructs.h"
#include "src/app/WindowFunc.h"
#include "resources/resource.h"

class SettDialog
{
public:
	void connectGraph(IObserverGraph *obj);

	void connectCommand(IObserverCommand *obj);

	void show(HWND hWnd);

private:
	void initHwnd(HWND hWnd);
	BOOL initDialog(HWND hwnd, COLORREF *colors, int &numberOfSamples);
	LONG_PTR setColor(HWND colorHwnd, HDC hdc);

private:
	static LRESULT CALLBACK SetupDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND HPB_Background;
	HWND HPB_Side;
	HWND HPB_Bottom;
	HWND HPB_Axis;
	HWND HPB_GridM;
	HWND HPB_GridS;
	HWND HPB_Graph;

	HWND HE_Graph_W;
	HWND HE_Axis_W;

	HWND HCB_WF;

	HWND HE_Sample;
	HWND HE_XSplit;
	HWND HE_YSplit;

};
