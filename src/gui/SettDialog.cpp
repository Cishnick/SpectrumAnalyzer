#include "src/app/stdafx.h"

#include "SettDialog.h"

#include <cassert>

IObserverGraph *graph = nullptr;
IObserverCommand *com = nullptr;
ColorsGraph sett;

using namespace Gdiplus;

constexpr const char *kWindowFunctionGuiNames[] = {"Синус-окно",
                                                   "Окно Ханна",
                                                   "Окно Барлетта-Ханна",
                                                   "Окно Хэмминга",
                                                   "Окно Блэкмана",
                                                   "Окно Блэкмана-Харриса",
                                                   "Окно Нэттала",
                                                   "Окно Блэкмана-Нэттала"};

constexpr auto kDefaultXSplits = "65";
constexpr auto kDefaultYSplits = "30";

LRESULT CALLBACK SetupDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace
{

BOOL setColorByDialog(Color &color, HWND hwnd)
{
	CHOOSECOLOR cc;
	cc.rgbResult = color.ToCOLORREF();
	ChooseColor(&cc);
	color.SetFromCOLORREF(cc.rgbResult);
	InvalidateRect(hwnd, nullptr, false);
	return TRUE;
}

template <class T>
BOOL setWidthFromEdit(T &width, HWND hwnd)
{
	constexpr auto kWindowTextBufferSize = 10;
	char buf[kWindowTextBufferSize];
	GetWindowText(hwnd, buf, kWindowTextBufferSize);
	width = std::stoi(buf);
	return TRUE;
}

} // namespace

enum class WindowFunctionIndex
{
	BartlettHann,
	Blackman,
	BlackmanNuttall,
	BlackmanHarris,
	Nuttall,
	Hann,
	Hamming,
	Sinus
};

void SettDialog::initHwnd(HWND hWnd)
{
	HPB_Background = GetDlgItem(hWnd, IDB_BACKGND);
	HPB_Side = GetDlgItem(hWnd, IDB_SIDE);
	HPB_Bottom = GetDlgItem(hWnd, IDB_BOTTOM);
	HPB_Axis = GetDlgItem(hWnd, IDB_AXIS);
	HPB_GridM = GetDlgItem(hWnd, IDB_MGRID);
	HPB_GridS = GetDlgItem(hWnd, IDB_SGRID);
	HPB_Graph = GetDlgItem(hWnd, IDB_GRAPH);

	HE_Graph_W = GetDlgItem(hWnd, IDE_GRAPHW);
	HE_Axis_W = GetDlgItem(hWnd, IDE_AXISW);

	HCB_WF = GetDlgItem(hWnd, IDCB_WF);

	HE_Sample = GetDlgItem(hWnd, IDE_SAMPLE);
	HE_XSplit = GetDlgItem(hWnd, IDE_XSPLIT);
	HE_YSplit = GetDlgItem(hWnd, IDE_YSPLIT);
}

void initWindowFunctionControl(HWND hwnd)
{
	for (auto wfName : kWindowFunctionGuiNames)
		SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)wfName);
	SendMessage(hwnd, CB_SETCURSEL, (WPARAM)6, (LPARAM)0);
}

BOOL SettDialog::initDialog(HWND hwnd, COLORREF *colors, int &numberOfSamples)
{
	initHwnd(hwnd);
	char buf[10];

	sprintf_s(buf, "%d", sett.graphWidth);
	SetWindowText(HE_Graph_W, buf);

	sprintf_s(buf, "%d", sett.axisWidth);
	SetWindowText(HE_Axis_W, buf);

	sprintf_s(buf, "%d", numberOfSamples);
	SetWindowText(HE_Sample, buf);
	SetWindowText(HE_XSplit, kDefaultXSplits);
	SetWindowText(HE_YSplit, kDefaultYSplits);

	initWindowFunctionControl(HCB_WF);
	return TRUE;
}

void CSInit()
{
	sett.backGroundColor = Color::AliceBlue;
	sett.axisColor = Color::Black;
	sett.bottomColor = Color::LightGreen;
	sett.graphColor = Color::Blue;
	sett.sideColor = Color::LightBlue;
	sett.gridMainColor = Color::DarkGray;
	sett.gridSecondColor = Color::DarkGray;
	sett.axisWidth = 1.f;
	sett.graphWidth = 2.f;
}

WindowFunctor chooseWindowFunction(WindowFunctionIndex index)
{
	switch (index)
	{
		case WindowFunctionIndex::BartlettHann: return WF::BartlettHann;
		case WindowFunctionIndex::Blackman: return WF::Blackman;
		case WindowFunctionIndex::BlackmanNuttall: return WF::BlackmanNuttall;
		case WindowFunctionIndex::BlackmanHarris: return WF::BlackmanHarris;
		case WindowFunctionIndex::Nuttall: return WF::Nuttall;
		case WindowFunctionIndex::Hann: return WF::Hann;
		case WindowFunctionIndex::Hamming: return WF::Hamming;
		case WindowFunctionIndex::Sinus: return WF::Sinus;
		default: assert(false && "Unknown window function index!");
	}
}

void SettDialog::connectGraph(IObserverGraph *obj)
{
	graph = obj;
	CSInit();
	graph->setColors(sett);
}

void SettDialog::connectCommand(IObserverCommand *obj)
{
	com = obj;
}

void SettDialog::show(HWND hWnd)
{
	DialogBoxParam((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
	               "SETTINGS_VIEW",
	               hWnd,
	               (DLGPROC)SettDialog::SetupDlgProc,
	               reinterpret_cast<LPARAM>(this));
}

LRESULT CALLBACK SettDialog::SetupDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COLORREF colors[16];
	static int numberOfSamples = 4800;

	SettDialog *instance;

	instance = reinterpret_cast<SettDialog *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (uMsg)
	{
		case WM_INITDIALOG:
			instance = reinterpret_cast<SettDialog *>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
			return instance->initDialog(hwnd, colors, numberOfSamples);
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDCANCEL: EndDialog(hwnd, static_cast<INT_PTR>(wParam)); return TRUE;
				case IDOK:
					graph->setColors(sett);
					com->setSamples(numberOfSamples);
					EndDialog(hwnd, static_cast<INT_PTR>(wParam));
					return TRUE;
				case IDB_BACKGND: return setColorByDialog(sett.backGroundColor, hwnd);
				case IDB_AXIS: return setColorByDialog(sett.axisColor, hwnd);
				case IDB_BOTTOM: return setColorByDialog(sett.bottomColor, hwnd);
				case IDB_MGRID: return setColorByDialog(sett.gridMainColor, hwnd);
				case IDB_SGRID: return setColorByDialog(sett.gridSecondColor, hwnd);
				case IDB_SIDE: return setColorByDialog(sett.sideColor, hwnd);
				case IDB_GRAPH: return setColorByDialog(sett.graphColor, hwnd);
				case IDE_GRAPHW:
					if (HIWORD(wParam) == EN_CHANGE)
						return setWidthFromEdit(sett.graphWidth, instance->HE_Graph_W);
					return FALSE;
				case IDE_AXISW:
					if (HIWORD(wParam) == EN_CHANGE)
						return setWidthFromEdit(sett.axisWidth, instance->HE_Axis_W);
					return FALSE;
				case IDCB_WF:
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						if (!com)
							return FALSE;
						com->setWindowFunction(chooseWindowFunction(
							static_cast<WindowFunctionIndex>(SendMessage(instance->HCB_WF, CB_GETCURSEL, 0, 0L))));
						return TRUE;
					}
					return FALSE;
				case IDE_SAMPLE:
					if (HIWORD(wParam) == EN_CHANGE)
						return setWidthFromEdit(numberOfSamples, instance->HE_Sample);
					return FALSE;
			}
			return FALSE;
		case WM_CTLCOLORBTN: return instance->setColor(reinterpret_cast<HWND>(lParam), reinterpret_cast<HDC>(wParam));
	}
	return FALSE;
}

LONG_PTR SettDialog::setColor(HWND colorHwnd, HDC hdc)
{
	Color color;
	if (colorHwnd == HPB_Background)
		color = sett.backGroundColor;
	else if (colorHwnd == HPB_Bottom)
		color = sett.bottomColor;
	else if (colorHwnd == HPB_Side)
		color = sett.sideColor;
	else if (colorHwnd == HPB_Axis)
		color = sett.axisColor;
	else if (colorHwnd == HPB_GridM)
		color = sett.gridMainColor;
	else if (colorHwnd == HPB_GridS)
		color = sett.gridSecondColor;
	else if (colorHwnd == HPB_Graph)
		color = sett.graphColor;
	else
		return FALSE;
	SetBkColor(hdc, color.ToCOLORREF());
	return reinterpret_cast<LONG_PTR>(CreateSolidBrush(color.ToCOLORREF()));
}
