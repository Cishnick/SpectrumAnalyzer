#include "stdafx.h"

#include "WndInitializer.h"

#include <objidl.h>

#define TEXT_X " Гц"
#define TEXT_Y "1e-3"
#define TEXT_DB " dB"

#define EDITS_X 1092
#define LABEL1_W 130
#define LABEL2_X 1177
#define LABEL2_W 30

#define TEXT_MIN "Мин. значение  :"
#define TEXT_MAX "Макс. значение :"
#define TEXT_SPL "Кол-во делений :"

#define GROUPBOX1_W 295
#define GROUPBOX2_W 272

#define IDB_ON 101
#define IDC_SELDEV 102

#define IDE_XMIN 201
#define IDE_XMAX 202
#define IDE_XSPLIT 203

#define IDE_YMIN 204
#define IDE_YMAX 205
#define IDE_YSPLIT 206

#define IDCH_XLOG 207
#define IDCH_YLOG 208

#define Y0 7

LRESULT CALLBACK XMinLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK XMaxLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK YMinLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK YMaxLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ------------------------Old EditProc functions-------------------------------------------
LONG_PTR OldXMinProc;
LONG_PTR OldXMaxProc;
LONG_PTR OldYMinProc;
LONG_PTR OldYMaxProc;
// --------------------------------------------------------------------------------------------

using namespace Gdiplus;

//-------------------------------Конструктор--------------------------------------------------------
GUI::GUI(size_t t_size, double f_step, int width, int heght)
	: _PaintRect()
	, _HWnd()
	, _GdiToken()
	, _GraphPoints(nullptr)
	, _NOfPoints(0)
	, _ClientRect(0, 0, width, heght)
	, _Graphic(_PaintRect, t_size, f_step, 0.01)
	, _MaxAmpl(1e-4)
	, _MaxFrenq(t_size * f_step)
{
	_SDialog.connectGraph(&_Graphic);
}

// -----------------------------------------------initialize----------------------------------------------------
int GUI::Initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine)
{
	const char *ClassName = "Program";
	GdiplusStartupInput gdiplusStartupInput;
	WNDCLASSEX wcx;

	GdiplusStartup(&_GdiToken, &gdiplusStartupInput, NULL);

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = lpWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInst;
	wcx.hIcon = LoadIcon(hInst, "IDI_ICON1");
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)COLOR_WINDOW; //static_cast<HBRUSH>(GetStockObject());
	wcx.lpszMenuName = "MAINMENU";
	wcx.lpszClassName = ClassName;
	wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcx))
	{
		ErrorMessageShow("Не удается зарегестрировать класс");
		return 1;
	}

	_HWnd = CreateWindow(ClassName,
	                     HeaderWindow.c_str(),
	                     WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	                     CW_USEDEFAULT,
	                     CW_USEDEFAULT,
	                     _ClientRect.Width,
	                     _ClientRect.Height,
	                     NULL,
	                     NULL,
	                     hInst,
	                     NULL);

	//-----------------------Create Settings Panel------------------------------------------------------

	// -------------------------------Combobox------------------------------------------
	_HComboSelectDev = CreateWindow("ComboBox",
	                                "",
	                                CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_SIMPLE,
	                                941,
	                                Y0 + 20,
	                                GROUPBOX2_W,
	                                80,
	                                _HWnd,
	                                (HMENU)IDC_SELDEV,
	                                hInst,
	                                NULL);

	//----------------------------------GroupBox-----------------------------------------------
	CreateWindowEx(0,
	               "BUTTON",
	               "Устройство захвата",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               930,
	               Y0 - 8,
	               GROUPBOX1_W,
	               70,
	               _HWnd,
	               NULL,
	               hInst,
	               NULL);

	CreateWindowEx(0,
	               "BUTTON",
	               "Отображение графика",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               930,
	               Y0 + 70,
	               GROUPBOX1_W,
	               320,
	               _HWnd,
	               NULL,
	               hInst,
	               NULL);

	CreateWindowEx(0,
	               "BUTTON",
	               "Ось Х",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               941,
	               Y0 + 90,
	               GROUPBOX2_W,
	               140,
	               _HWnd,
	               NULL,
	               hInst,
	               NULL);

	CreateWindowEx(0,
	               "BUTTON",
	               "Ось У",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               941,
	               Y0 + 238,
	               GROUPBOX2_W,
	               140,
	               _HWnd,
	               NULL,
	               hInst,
	               NULL);
	//------------------------------------------XParametres----------------------------------------

	// ---------------------------------------------------XMIN------------------------------------
	_HEditXMin = CreateWindow("edit",
	                          "0",
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                          EDITS_X,
	                          Y0 + 110,
	                          82,
	                          20,
	                          _HWnd,
	                          (HMENU)IDE_XMIN,
	                          hInst,
	                          NULL);

	HWND label1 = CreateWindow("static",
	                           TEXT_MIN,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 113,
	                           LABEL1_W,
	                           20,
	                           _HWnd,
	                           NULL,
	                           hInst,
	                           NULL);

	HWND label11 = CreateWindow("static",
	                            TEXT_X,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            LABEL2_X,
	                            Y0 + 113,
	                            LABEL2_W,
	                            20,
	                            _HWnd,
	                            NULL,
	                            hInst,
	                            NULL);
	// ------------------------------------------------------------X Max---------------------------
	_HEditXMax = CreateWindow("edit",
	                          "0",
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                          EDITS_X,
	                          Y0 + 140,
	                          82,
	                          20,
	                          _HWnd,
	                          (HMENU)IDE_XMAX,
	                          hInst,
	                          NULL);

	HWND label2 = CreateWindow("static",
	                           TEXT_MAX,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 143,
	                           LABEL1_W,
	                           20,
	                           _HWnd,
	                           NULL,
	                           hInst,
	                           NULL);

	HWND label22 = CreateWindow("static",
	                            TEXT_X,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            LABEL2_X,
	                            Y0 + 143,
	                            LABEL2_W,
	                            20,
	                            _HWnd,
	                            NULL,
	                            hInst,
	                            NULL);
	// -----------------------------------------------X Step------------------------------------------
	_HEditXSplit = CreateWindow("edit",
	                            "0",
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL | ES_NUMBER,
	                            EDITS_X,
	                            Y0 + 170,
	                            82,
	                            20,
	                            _HWnd,
	                            (HMENU)IDE_XSPLIT,
	                            hInst,
	                            NULL);

	HWND label3 = CreateWindow("static",
	                           TEXT_SPL,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 173,
	                           LABEL1_W,
	                           20,
	                           _HWnd,
	                           NULL,
	                           hInst,
	                           NULL);
	/*
		HWND label33 = CreateWindow("static", TEXT_X,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			LABEL2_X, Y0 + 173, LABEL2_W, 20, _HWnd, NULL, hInst, NULL);*/

	// ------------------------------------------CheckBox LOG-----------------------------------------------
	_HCheckXLog = CreateWindowEx(0,
	                             "BUTTON",
	                             "Log",
	                             WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	                             952,
	                             Y0 + 200,
	                             50,
	                             20,
	                             _HWnd,
	                             (HMENU)IDCH_XLOG,
	                             hInst,
	                             NULL);

	//-----------------------------Y Parametres------------------------------------------------------------

	// ---------------------------------------------------Y Min--------------------------------
	_HEditYMin = CreateWindow("edit",
	                          "0",
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                          EDITS_X,
	                          Y0 + 258,
	                          82,
	                          20,
	                          _HWnd,
	                          (HMENU)IDE_YMIN,
	                          hInst,
	                          NULL);

	HWND label12 = CreateWindow("static",
	                            TEXT_MIN,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            952,
	                            Y0 + 261,
	                            LABEL1_W,
	                            20,
	                            _HWnd,
	                            NULL,
	                            hInst,
	                            NULL);

	_HLabelYMin = CreateWindow("static",
	                           TEXT_Y,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           LABEL2_X,
	                           Y0 + 261,
	                           LABEL2_W,
	                           20,
	                           _HWnd,
	                           NULL,
	                           hInst,
	                           NULL);
	// ------------------------------------------------------------Y Max---------------------------
	_HEditYMax = CreateWindow("edit",
	                          "0",
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                          EDITS_X,
	                          Y0 + 288,
	                          82,
	                          20,
	                          _HWnd,
	                          (HMENU)IDE_YMAX,
	                          hInst,
	                          NULL);

	HWND label2_2 = CreateWindow("static",
	                             TEXT_MAX,
	                             WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                             952,
	                             Y0 + 291,
	                             LABEL1_W,
	                             20,
	                             _HWnd,
	                             NULL,
	                             hInst,
	                             NULL);

	_HLabelYMax = CreateWindow("static",
	                           TEXT_Y,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           LABEL2_X,
	                           Y0 + 291,
	                           LABEL2_W,
	                           20,
	                           _HWnd,
	                           NULL,
	                           hInst,
	                           NULL);
	// -----------------------------------------------Y Step------------------------------------------
	_HEditYSplit = CreateWindow("edit",
	                            "0",
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL | ES_NUMBER,
	                            EDITS_X,
	                            Y0 + 318,
	                            82,
	                            20,
	                            _HWnd,
	                            (HMENU)IDE_YSPLIT,
	                            hInst,
	                            NULL);

	HWND label32 = CreateWindow("static",
	                            TEXT_SPL,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            952,
	                            Y0 + 321,
	                            LABEL1_W,
	                            20,
	                            _HWnd,
	                            NULL,
	                            hInst,
	                            NULL);

	//HWND label332 = CreateWindow("static", TEXT_Y,
	//	WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	//	LABEL2_X, Y0 + 321, LABEL2_W, 20, _HWnd, NULL, hInst, NULL);
	// ------------------------------------------CheckBox LOG-----------------------------------------------
	_HCheckYLog = CreateWindowEx(0,
	                             "BUTTON",
	                             "Log",
	                             WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	                             952,
	                             Y0 + 351,
	                             50,
	                             20,
	                             _HWnd,
	                             (HMENU)IDCH_YLOG,
	                             hInst,
	                             NULL);
	//------------------------------------------END---------------------------------------------------------

	//--------------------------------- ON Button-----------------------------------------------------------
	auto hwnd_ok = CreateWindow("Button",
	                            "On",
	                            WS_CHILD | WS_VISIBLE | WS_OVERLAPPED,
	                            929,
	                            Y0 + 393,
	                            GROUPBOX1_W,
	                            27,
	                            _HWnd,
	                            (HMENU)IDB_ON,
	                            hInst,
	                            NULL);

	char buf[10] = {'\0'};
	sprintf_s(buf, "%d", (int)_MaxFrenq);
	SetWindowText(_HEditXMax, buf);
	SetWindowText(_HEditXSplit, "10");

	sprintf_s(buf, "%g", (_MaxAmpl * 1e3));
	SetWindowText(_HEditYMax, buf);
	SetWindowText(_HEditYSplit, "10");

	// ---------------------Set EditProc Function--------------------------------------------------
	OldXMinProc = (LONG_PTR)SetWindowLongPtr(_HEditXMin, GWLP_WNDPROC, (LONG_PTR)XMinLProc);
	OldXMaxProc = (LONG_PTR)SetWindowLongPtr(_HEditXMax, GWLP_WNDPROC, (LONG_PTR)XMaxLProc);
	OldYMinProc = (LONG_PTR)SetWindowLongPtr(_HEditYMin, GWLP_WNDPROC, (LONG_PTR)YMinLProc);
	OldYMaxProc = (LONG_PTR)SetWindowLongPtr(_HEditYMax, GWLP_WNDPROC, (LONG_PTR)YMaxLProc);
	//------------------------------------------------------------------------------------------------

	ShowWindow(_HWnd, szCmdLine);
	UpdateWindow(_HWnd);
	return 0;
}

// --------------------------------------------Create Handler------------------------------------------
void GUI::CreateHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd)
{
	CreatePaintArea(width, height, x_l, y_l, hWnd);
}

// ------------------------------------------Create PaintArea------------------------------------------
void GUI::CreatePaintArea(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd)
{
	_PaintRect = Rect(x_l, y_l, width, height);
	_PaintRECT.bottom = y_l + height;
	_PaintRECT.left = x_l;
	_PaintRECT.right = x_l + width;
	_PaintRECT.top = y_l;
	_Graphic.Resize(_PaintRect);
	InvalidateRect(hWnd, &_PaintRECT, false);
}

// ---------------------------------------------Rendering--------------------------------------------------
void GUI::Rendering(HWND hWnd)
{
	_HWnd = hWnd;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	OnPaint(hdc);
	EndPaint(hWnd, &ps);
	if (_GraphPoints != NULL)
	{
		delete[] _GraphPoints;
		_GraphPoints = NULL;
	}
}

// --------------------------------------------------------------DrawSpectr-------------------------------
void GUI::DrawSpectr(Reals &R, Reals &L, ChannelsMode cmode, GraphMode gmode)
{
	_Graphic.SetData(R);
	InvalidateRect(_HWnd, &_PaintRECT, false);
}

// ----------------------------------------Release--------------------------------------------------------
void GUI::Release()
{
	GdiplusShutdown(_GdiToken);
}

// ---------------------------------------------------ChangeSize--------------------------------------------
void GUI::ChangeSize(int width, int height)
{
	static double dw = _ClientRect.Width / (double)_PaintRect.Width;
	static double dh = _ClientRect.Height / (double)_PaintRect.Height;
	_ClientRect.Width = width;
	_ClientRect.Height = height;
	_PaintRect.Width = width / dw;
	_PaintRect.Height = height / dh;
}

// -------------------------------------------------commandHandler---------------------------------------------
void GUI::CommandHandler(WPARAM wParam, LPARAM lParam)
{
	char buf[10];
	double temp;
	switch (LOWORD(wParam))
	{
		case IDB_ON:
		{
			static bool f = true;
			Switch(f);
			f = !f;
			SetWindowText((HWND)lParam, f ? "On" : "Off");
			return;
		}
		case IDC_SELDEV:
		{
			switch (HIWORD(wParam))
			{
				case CBN_SELCHANGE: int index = (int)SendMessage(_HComboSelectDev, CB_GETCURSEL, 0, 0L); ChangeDev(index);
			}
			return;
		}
		case IDE_XMIN:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditXMin, buf, 10);
					temp = atof(buf);
					_Graphic.SetXMin(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}

			return;
		}
		case IDE_XMAX:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditXMax, buf, 10);
					temp = atof(buf);
					if (temp > _MaxFrenq)
						temp = _MaxFrenq;
					_Graphic.SetXMax(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}

			return;
		}
		case IDE_XSPLIT:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditXSplit, buf, 10);
					temp = atof(buf);
					_Graphic.SetXSplit(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}

			return;
		}
		case IDE_YMIN:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditYMin, buf, 10);
					temp = atof(buf);
					_Graphic.SetYMin(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}

			return;
		}
		case IDE_YMAX:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditYMax, buf, 10);
					temp = atof(buf);
					_Graphic.SetYMax(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}

			return;
		}
		case IDE_YSPLIT:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(_HEditYSplit, buf, 10);
					temp = atof(buf);
					_Graphic.SetYSplit(temp);
					InvalidateRect(_HWnd, &_PaintRECT, false);
					return;
				}
			}
			return;
		}
		case IDCH_XLOG:
		{
			auto state = SendMessage(_HCheckXLog, BM_GETCHECK, 0, (LPARAM)0);
			std::pair<double, double> t;
			char buf[10] = {'\0'};
			if (state == BST_CHECKED)
				t = _Graphic.SetLogX(true);
			else if (state == BST_UNCHECKED)
				t = _Graphic.SetLogX(false);

			sprintf_s(buf, "%d", (int)t.first);
			SetWindowText(_HEditXMin, buf);
			sprintf_s(buf, "%d", (int)t.second);
			SetWindowText(_HEditXMax, buf);

			InvalidateRect(_HWnd, &_PaintRECT, false);
			return;
		}
		case IDCH_YLOG:
		{
			auto state = SendMessage(_HCheckYLog, BM_GETCHECK, 0, (LPARAM)0);
			std::pair<double, double> t;
			char buf[10] = {'\0'};
			if (state == BST_CHECKED)
			{
				t = _Graphic.SetLogY(true);
				SetWindowText(_HLabelYMin, TEXT_DB);
				SetWindowText(_HLabelYMax, TEXT_DB);
			}
			else if (state == BST_UNCHECKED)
			{
				t = _Graphic.SetLogY(false);
				SetWindowText(_HLabelYMin, TEXT_Y);
				SetWindowText(_HLabelYMax, TEXT_Y);
			}

			sprintf_s(buf, "%g", t.first);
			SetWindowText(_HEditYMin, buf);
			sprintf_s(buf, "%g", t.second);
			SetWindowText(_HEditYMax, buf);

			InvalidateRect(_HWnd, &_PaintRECT, false);
			return;
		}
		// -------------------------------------------------Меню -------------------------------------------
		case IDM_EXIT:
		{
			DestroyWindow(_HWnd);
			return;
		}
		case IDM_SETTINGS:
		{
			_SDialog.Show(_HWnd);
			return;
		}
		case IDM_ABOUT:
		{
			MessageBox(_HWnd,
			           "\n\
Зубашевский Н.М. РЭ3-82\n\
2019\n\
			\n",
			           "О программе",
			           MB_OK);
			return;
		}
	}
}

// -----------------------------------------------------add-----------------------------------------------
void GUI::add(IObserverCommand &obs)
{
	_observers.push_back(&obs);
}

// -------------------------------------------------------remove-----------------------------
void GUI::remove(IObserverCommand &obs)
{
	_observers.remove(&obs);
}

// ------------------------------------Switch event----------------------------------------------
void GUI::Switch(bool state)
{
	for (auto i : _observers)
	{
		i->Switch(state);
	}
}

// ------------------------------------------ChangeDev event-------------------------------------
void GUI::ChangeDev(int index)
{
	for (auto i : _observers)
	{
		i->changeDevice(index);
	}
}

// --------------------------------------------------------SetDevList------------------------------------------
void GUI::setDevList(VectorOfString const &devs)
{
	for (auto name : devs)
	{
		SendMessage(_HComboSelectDev, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)name.c_str());
	}

	SendMessage(_HComboSelectDev, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

void GUI::connectCom(IObserverCommand *obj)
{
	_SDialog.connectCommand(obj);
}

// -------------------------------------------------SetPacketSize-----------------------------------
void GUI::SetPacketSize(int n, double f_step)
{
	_MaxFrenq = n * f_step;
	_Graphic.SetSampleN(n, f_step);
}

// ----------------------------------------------OnPaint--------------------------------------------
void GUI::OnPaint(HDC hdc)
{
	_Graphic.Rendering(hdc);
}

// -----------------------------------Filter--------------------------------------------------------
int WM_CHARFilter(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	static char buf[10]{0};
	char sett = static_cast<char>(wParam);
	int n = GetWindowText(hWnd, buf, 10);
	bool is_dot = !strchr(buf, '.');
	bool is_minus = !strchr(buf, '-');
	if (!(isdigit(sett) || sett == '\b' || (sett == '.' && is_dot) || (sett == '-' && is_minus)))
		return 0;
	else
		return 1;
}

// -----------------------------------XMin Edit Process function------------------------------------------------
LRESULT CALLBACK XMinLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CHAR:
		{
			if (!WM_CHARFilter(hWnd, wParam, lParam))
			{
				return 0;
			}
		}
	}
	return ((LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM))OldXMinProc)(hWnd, uMsg, wParam, lParam);
}

// -----------------------------------XMax Edit Process function------------------------------------------------
LRESULT CALLBACK XMaxLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[10];
	switch (uMsg)
	{
		case WM_CHAR:
		{
			if (!WM_CHARFilter(hWnd, wParam, lParam))
			{
				return 0;
			}
		}
	}
	return ((LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM))OldXMaxProc)(hWnd, uMsg, wParam, lParam);
}

// -----------------------------------YMin Edit Process function------------------------------------------------
LRESULT CALLBACK YMinLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[10];
	switch (uMsg)
	{
		case WM_CHAR:
		{
			if (!WM_CHARFilter(hWnd, wParam, lParam))
			{
				return 0;
			}
		}
	}
	return ((LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM))OldYMinProc)(hWnd, uMsg, wParam, lParam);
}

// -----------------------------------XMin Edit Process function------------------------------------------------
LRESULT CALLBACK YMaxLProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buf[10];
	switch (uMsg)
	{
		case WM_CHAR:
		{
			if (!WM_CHARFilter(hWnd, wParam, lParam))
			{
				return 0;
			}
		}
	}
	return ((LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM))OldYMaxProc)(hWnd, uMsg, wParam, lParam);
}

// --------------------------------Factory::make------------------------------------------------------
IGUI *GUIFactory::make(size_t t_size, double f_step, int width, int heght)
{
	return new GUI(t_size, f_step, width, heght);
}
