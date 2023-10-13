#include "src/app/stdafx.h"

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

namespace
{

constexpr auto kMenuResourceName = "MAINMENU";

RECT gdiRect2WinapiRect(const Gdiplus::Rect &rect)
{
	return RECT{rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
}

} // namespace

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

GUI::GUI(size_t samplesNumber, double freqStep, int width, int height)
	: paintRect()
	, hwnd()
	, gdiToken()
	, graphPoints(nullptr)
	, clientRect(0, 0, width, height)
	, graphBox(paintRect, samplesNumber, freqStep, 0.01)
	, maxAmpl(1e-4)
	, maxFreq(samplesNumber * freqStep)
{
	settingsDialog.connectGraph(&graphBox);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiToken, &gdiplusStartupInput, nullptr);
}

int GUI::initialize(string HeaderWindow, WNDPROC lpWndProc, HINSTANCE hInst, int szCmdLine)
{
	constexpr auto kClassName = "Program";
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = lpWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInst;
	wcx.hIcon = nullptr;
	wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcx.lpszMenuName = kMenuResourceName;
	wcx.lpszClassName = kClassName;
	wcx.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wcx))
		throw std::exception("Не удается зарегестрировать класс");

	hwnd = CreateWindow(kClassName,
	                    HeaderWindow.c_str(),
	                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	                    CW_USEDEFAULT,
	                    CW_USEDEFAULT,
	                    clientRect.Width,
	                    clientRect.Height,
	                    nullptr,
	                    nullptr,
	                    hInst,
	                    nullptr);

	//-----------------------Create Settings Panel------------------------------------------------------

	// -------------------------------Combobox------------------------------------------
	hComboSelectDev = CreateWindow("ComboBox",
	                               "",
	                               CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_SIMPLE,
	                               941,
	                               Y0 + 20,
	                               GROUPBOX2_W,
	                               80,
	                               hwnd,
	                               (HMENU)IDC_SELDEV,
	                               hInst,
	                               nullptr);

	//----------------------------------GroupBox-----------------------------------------------
	CreateWindowEx(0,
	               "BUTTON",
	               "Устройство захвата",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               930,
	               Y0 - 8,
	               GROUPBOX1_W,
	               70,
	               hwnd,
	               nullptr,
	               hInst,
	               nullptr);

	CreateWindowEx(0,
	               "BUTTON",
	               "Отображение графика",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               930,
	               Y0 + 70,
	               GROUPBOX1_W,
	               320,
	               hwnd,
	               nullptr,
	               hInst,
	               nullptr);

	CreateWindowEx(0,
	               "BUTTON",
	               "Ось Х",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               941,
	               Y0 + 90,
	               GROUPBOX2_W,
	               140,
	               hwnd,
	               nullptr,
	               hInst,
	               nullptr);

	CreateWindowEx(0,
	               "BUTTON",
	               "Ось У",
	               WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	               941,
	               Y0 + 238,
	               GROUPBOX2_W,
	               140,
	               hwnd,
	               nullptr,
	               hInst,
	               nullptr);
	//------------------------------------------XParametres----------------------------------------

	// ---------------------------------------------------XMIN------------------------------------
	hEditXMin = CreateWindow("edit",
	                         "0",
	                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                         EDITS_X,
	                         Y0 + 110,
	                         82,
	                         20,
	                         hwnd,
	                         (HMENU)IDE_XMIN,
	                         hInst,
	                         nullptr);

	HWND label1 = CreateWindow("static",
	                           TEXT_MIN,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 113,
	                           LABEL1_W,
	                           20,
	                           hwnd,
	                           nullptr,
	                           hInst,
	                           nullptr);

	HWND label11 = CreateWindow("static",
	                            TEXT_X,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            LABEL2_X,
	                            Y0 + 113,
	                            LABEL2_W,
	                            20,
	                            hwnd,
	                            nullptr,
	                            hInst,
	                            nullptr);
	// ------------------------------------------------------------X Max---------------------------
	hEditXMax = CreateWindow("edit",
	                         "0",
	                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                         EDITS_X,
	                         Y0 + 140,
	                         82,
	                         20,
	                         hwnd,
	                         (HMENU)IDE_XMAX,
	                         hInst,
	                         nullptr);

	HWND label2 = CreateWindow("static",
	                           TEXT_MAX,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 143,
	                           LABEL1_W,
	                           20,
	                           hwnd,
	                           nullptr,
	                           hInst,
	                           nullptr);

	HWND label22 = CreateWindow("static",
	                            TEXT_X,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            LABEL2_X,
	                            Y0 + 143,
	                            LABEL2_W,
	                            20,
	                            hwnd,
	                            nullptr,
	                            hInst,
	                            nullptr);
	// -----------------------------------------------X Step------------------------------------------
	hEditXSplit = CreateWindow("edit",
	                           "0",
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL | ES_NUMBER,
	                           EDITS_X,
	                           Y0 + 170,
	                           82,
	                           20,
	                           hwnd,
	                           (HMENU)IDE_XSPLIT,
	                           hInst,
	                           nullptr);

	HWND label3 = CreateWindow("static",
	                           TEXT_SPL,
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                           952,
	                           Y0 + 173,
	                           LABEL1_W,
	                           20,
	                           hwnd,
	                           nullptr,
	                           hInst,
	                           nullptr);
	/*
		HWND label33 = CreateWindow("static", TEXT_X,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			LABEL2_X, Y0 + 173, LABEL2_W, 20, hwnd, nullptr, hInst, nullptr);*/

	// ------------------------------------------CheckBox LOG-----------------------------------------------
	hCheckXLog = CreateWindowEx(0,
	                            "BUTTON",
	                            "Log",
	                            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	                            952,
	                            Y0 + 200,
	                            50,
	                            20,
	                            hwnd,
	                            (HMENU)IDCH_XLOG,
	                            hInst,
	                            nullptr);

	//-----------------------------Y Parametres------------------------------------------------------------

	// ---------------------------------------------------Y Min--------------------------------
	hEditYMin = CreateWindow("edit",
	                         "0",
	                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                         EDITS_X,
	                         Y0 + 258,
	                         82,
	                         20,
	                         hwnd,
	                         (HMENU)IDE_YMIN,
	                         hInst,
	                         nullptr);

	HWND label12 = CreateWindow("static",
	                            TEXT_MIN,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            952,
	                            Y0 + 261,
	                            LABEL1_W,
	                            20,
	                            hwnd,
	                            nullptr,
	                            hInst,
	                            nullptr);

	hLabelYMin = CreateWindow("static",
	                          TEXT_Y,
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                          LABEL2_X,
	                          Y0 + 261,
	                          LABEL2_W,
	                          20,
	                          hwnd,
	                          nullptr,
	                          hInst,
	                          nullptr);
	// ------------------------------------------------------------Y Max---------------------------
	hEditYMax = CreateWindow("edit",
	                         "0",
	                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL, // | ES_NUMBER,
	                         EDITS_X,
	                         Y0 + 288,
	                         82,
	                         20,
	                         hwnd,
	                         (HMENU)IDE_YMAX,
	                         hInst,
	                         nullptr);

	HWND label2_2 = CreateWindow("static",
	                             TEXT_MAX,
	                             WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                             952,
	                             Y0 + 291,
	                             LABEL1_W,
	                             20,
	                             hwnd,
	                             nullptr,
	                             hInst,
	                             nullptr);

	hLabelYMax = CreateWindow("static",
	                          TEXT_Y,
	                          WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                          LABEL2_X,
	                          Y0 + 291,
	                          LABEL2_W,
	                          20,
	                          hwnd,
	                          nullptr,
	                          hInst,
	                          nullptr);
	// -----------------------------------------------Y Step------------------------------------------
	hEditYSplit = CreateWindow("edit",
	                           "0",
	                           WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT | ES_AUTOVSCROLL | ES_NUMBER,
	                           EDITS_X,
	                           Y0 + 318,
	                           82,
	                           20,
	                           hwnd,
	                           (HMENU)IDE_YSPLIT,
	                           hInst,
	                           nullptr);

	HWND label32 = CreateWindow("static",
	                            TEXT_SPL,
	                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	                            952,
	                            Y0 + 321,
	                            LABEL1_W,
	                            20,
	                            hwnd,
	                            nullptr,
	                            hInst,
	                            nullptr);

	//HWND label332 = CreateWindow("static", TEXT_Y,
	//	WS_CHILD | WS_VISIBLE | WS_TABSTOP,
	//	LABEL2_X, Y0 + 321, LABEL2_W, 20, hwnd, nullptr, hInst, nullptr);
	// ------------------------------------------CheckBox LOG-----------------------------------------------
	hCheckYLog = CreateWindowEx(0,
	                            "BUTTON",
	                            "Log",
	                            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	                            952,
	                            Y0 + 351,
	                            50,
	                            20,
	                            hwnd,
	                            (HMENU)IDCH_YLOG,
	                            hInst,
	                            nullptr);
	//------------------------------------------END---------------------------------------------------------

	//--------------------------------- ON Button-----------------------------------------------------------
	auto hwnd_ok = CreateWindow("Button",
	                            "On",
	                            WS_CHILD | WS_VISIBLE | WS_OVERLAPPED,
	                            929,
	                            Y0 + 393,
	                            GROUPBOX1_W,
	                            27,
	                            hwnd,
	                            (HMENU)IDB_ON,
	                            hInst,
	                            nullptr);

	char buf[10] = {'\0'};
	sprintf_s(buf, "%d", (int)maxFreq);
	SetWindowText(hEditXMax, buf);
	SetWindowText(hEditXSplit, "10");

	sprintf_s(buf, "%g", (maxAmpl * 1e3));
	SetWindowText(hEditYMax, buf);
	SetWindowText(hEditYSplit, "10");

	// ---------------------Set EditProc Function--------------------------------------------------
	OldXMinProc = (LONG_PTR)SetWindowLongPtr(hEditXMin, GWLP_WNDPROC, (LONG_PTR)XMinLProc);
	OldXMaxProc = (LONG_PTR)SetWindowLongPtr(hEditXMax, GWLP_WNDPROC, (LONG_PTR)XMaxLProc);
	OldYMinProc = (LONG_PTR)SetWindowLongPtr(hEditYMin, GWLP_WNDPROC, (LONG_PTR)YMinLProc);
	OldYMaxProc = (LONG_PTR)SetWindowLongPtr(hEditYMax, GWLP_WNDPROC, (LONG_PTR)YMaxLProc);
	//------------------------------------------------------------------------------------------------

	ShowWindow(hwnd, szCmdLine);
	UpdateWindow(hwnd);
	return 0;
}

void GUI::createHandler(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd)
{
	createPaintArea(width, height, x_l, y_l, hWnd);
}

void GUI::createPaintArea(LONG width, LONG height, LONG x_l, LONG y_l, HWND hWnd)
{
	paintRect = Rect(x_l, y_l, width, height);
	graphBox.resize(paintRect);
	invalidatePaintRect();
}

void GUI::rendering(HWND hWnd)
{
	hwnd = hWnd;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	OnPaint(hdc);
	EndPaint(hWnd, &ps);
	if (graphPoints != nullptr)
	{
		delete[] graphPoints;
		graphPoints = nullptr;
	}
}

void GUI::drawSpectr(std::span<double> data)
{
	graphBox.setData(data);
	invalidatePaintRect();
}

void GUI::changeSize(int width, int height)
{
	static double dw = clientRect.Width / (double)paintRect.Width;
	static double dh = clientRect.Height / (double)paintRect.Height;
	clientRect.Width = width;
	clientRect.Height = height;
	paintRect.Width = static_cast<int>(round(width / dw));
	paintRect.Height = static_cast<int>(round(height / dh));
}

void GUI::commandHandler(WPARAM wParam, LPARAM lParam)
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
				case CBN_SELCHANGE: int index = (int)SendMessage(hComboSelectDev, CB_GETCURSEL, 0, 0L); changeDevice(index);
			}
			return;
		}
		case IDE_XMIN:
		{
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					GetWindowText(hEditXMin, buf, 10);
					temp = atof(buf);
					graphBox.setXMin(temp);
					invalidatePaintRect();
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
					GetWindowText(hEditXMax, buf, 10);
					temp = atof(buf);
					if (temp > maxFreq)
						temp = maxFreq;
					graphBox.setXMax(temp);
					invalidatePaintRect();
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
					GetWindowText(hEditXSplit, buf, 10);
					temp = atof(buf);
					graphBox.setXSplit(temp);
					invalidatePaintRect();
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
					GetWindowText(hEditYMin, buf, 10);
					temp = atof(buf);
					graphBox.setYMin(temp);
					invalidatePaintRect();
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
					GetWindowText(hEditYMax, buf, 10);
					temp = atof(buf);
					graphBox.setYMax(temp);
					invalidatePaintRect();
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
					GetWindowText(hEditYSplit, buf, 10);
					temp = atof(buf);
					graphBox.setYSplit(temp);
					invalidatePaintRect();
					return;
				}
			}
			return;
		}
		case IDCH_XLOG:
		{
			auto state = SendMessage(hCheckXLog, BM_GETCHECK, 0, (LPARAM)0);
			std::pair<double, double> t;
			char buf[10] = {'\0'};
			if (state == BST_CHECKED)
				t = graphBox.setLogX(true);
			else if (state == BST_UNCHECKED)
				t = graphBox.setLogX(false);

			sprintf_s(buf, "%d", (int)t.first);
			SetWindowText(hEditXMin, buf);
			sprintf_s(buf, "%d", (int)t.second);
			SetWindowText(hEditXMax, buf);

			invalidatePaintRect();
			return;
		}
		case IDCH_YLOG:
		{
			auto state = SendMessage(hCheckYLog, BM_GETCHECK, 0, (LPARAM)0);
			std::pair<double, double> t;
			char buf[10] = {'\0'};
			if (state == BST_CHECKED)
			{
				t = graphBox.setLogY(true);
				SetWindowText(hLabelYMin, TEXT_DB);
				SetWindowText(hLabelYMax, TEXT_DB);
			}
			else if (state == BST_UNCHECKED)
			{
				t = graphBox.setLogY(false);
				SetWindowText(hLabelYMin, TEXT_Y);
				SetWindowText(hLabelYMax, TEXT_Y);
			}

			sprintf_s(buf, "%g", t.first);
			SetWindowText(hEditYMin, buf);
			sprintf_s(buf, "%g", t.second);
			SetWindowText(hEditYMax, buf);

			invalidatePaintRect();
			return;
		}
		// -------------------------------------------------Меню -------------------------------------------
		case IDM_EXIT:
		{
			DestroyWindow(hwnd);
			return;
		}
		case IDM_SETTINGS:
		{
			settingsDialog.show(hwnd);
			return;
		}
		case IDM_ABOUT:
		{
			MessageBox(hwnd,
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

// ------------------------------------switchCapturing event----------------------------------------------
void GUI::Switch(bool state)
{
	for (auto i : _observers)
	{
		i->switchCapturing(state);
	}
}

// ------------------------------------------changeDevice event-------------------------------------
void GUI::changeDevice(int index)
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
		SendMessage(hComboSelectDev, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)name.c_str());
	}

	SendMessage(hComboSelectDev, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

void GUI::connectCom(IObserverCommand *obj)
{
	settingsDialog.connectCommand(obj);
}

// -------------------------------------------------setPacketSize-----------------------------------
void GUI::setPacketSize(int n, double f_step)
{
	maxFreq = n * f_step;
	graphBox.setSampleN(n, f_step);
}

// ----------------------------------------------OnPaint--------------------------------------------
void GUI::OnPaint(HDC hdc)
{
	graphBox.rendering(hdc);
}

void GUI::invalidatePaintRect()
{
	InvalidateRect(hwnd, nullptr, false);
}

GUI::~GUI()
{
	GdiplusShutdown(gdiToken);
}

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
