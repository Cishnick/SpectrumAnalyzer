#include "stdafx.h"

#include "SettDialog.h"

IObserverGraph *graph = NULL;
IObserverCommand *com = NULL;
ColorsGraph sett;

bool *w;

using namespace Gdiplus;

// ---------------------------------------------w_str------------------------------------------------
string wf_str[] = {"Синус-окно",
                   "Окно Ханна",
                   "Окно Барлетта-Ханна",
                   "Окно Хэмминга",
                   "Окно Блэкмана",
                   "Окно Блэкмана-Харриса",
                   "Окно Нэттала",
                   "Окно Блэкмана-Нэттала"};

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

LRESULT CALLBACK SetupDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ----------------------------------------CSInit ---------------------------------------------------
void CSInit()
{
	sett.BackGroundColor = Color::AliceBlue;
	sett.AxisColor = Color::Black;
	sett.BottomColor = Color::LightGreen;
	sett.GraphColor = Color::Blue;
	sett.SideColor = Color::LightBlue;
	sett.GridMainColor = Color::DarkGray;
	sett.GridSecondColor = Color::DarkGray;
	sett.AxisWidth = 1.f;
	sett.GraphWidth = 2.f;
}

// -------------------------------------Init HWND ---------------------------------------------------
void InitHWND(HWND hWnd)
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

// ---------------------------------------------InitWF -----------------------------------------------
void InitWF()
{
	for (auto i : wf_str)
	{
		SendMessage(HCB_WF, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)i.c_str());
	}
	SendMessage(HCB_WF, CB_SETCURSEL, (WPARAM)6, (LPARAM)0);
}

// ---------------------------------------ChooseWF -----------------------------------------------
fd_t ChooseWF(int i)
{
	switch (i)
	{
		case 0: return WF::BartlettHann;
		case 1: return WF::Blackman;
		case 2: return WF::BlackmanNuttall;
		case 3: return WF::BlackmanHarris;
		case 4: return WF::Nuttall;
		case 5: return WF::Hann;
		case 6: return WF::Hamming;
		case 7: return WF::Sinus;
		default: return WF::Hamming;
	}
}

// ----------------------------------------connectGraph--------------------------------------------
void SettDialog::connectGraph(IObserverGraph *obj)
{
	graph = obj;
	CSInit();
	graph->SetColors(sett);
}

// --------------------------------------------------connectCommand---------------------------------
void SettDialog::connectCommand(IObserverCommand *obj)
{
	com = obj;
}

// -----------------------------------------Show ---------------------------------------------------
void SettDialog::Show(HWND hWnd)
{
	w = &_work;
	DialogBox((HINSTANCE)GetWindowLong(hWnd, -6), "SETTINGS_VIEW", hWnd, (DLGPROC)SetupDlgProc);
}

// ---------------------------------------------isWork ----------------------------------------------
bool SettDialog::isWork()
{
	return *w;
}

// -----------------------------------------------SetupDlgProc --------------------------------------
LRESULT CALLBACK SetupDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COLORREF colors[16];
	static CHOOSECOLOR cc;
	static int samples = 4800;
	switch (uMsg)
	{
			// ------------------------------------WM_INITDIALOG---------------------------------------
		case WM_INITDIALOG:
		{
			InitHWND(hwnd);
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hwnd;
			cc.lpCustColors = colors;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			char buf[10];

			sprintf_s(buf, "%d", (int)sett.GraphWidth);
			SetWindowText(HE_Graph_W, buf);

			sprintf_s(buf, "%d", (int)sett.AxisWidth);
			SetWindowText(HE_Axis_W, buf);

			sprintf_s(buf, "%d", samples);
			SetWindowText(HE_Sample, buf);
			SetWindowText(HE_XSplit, "65");
			SetWindowText(HE_YSplit, "30");

			InitWF();
			*w = true;
			return TRUE;
		}

		// ----------------------------WM_COMMAND --------------------------------------------------------
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
					// -------------------------------------------IDCANCEL -----------------------------------
				case IDCANCEL:
					EndDialog(hwnd, wParam);
					*w = false;
					return TRUE;
					// -------------------------------------IDOK ---------------------------------------------
				case IDOK:
					graph->SetColors(sett);
					com->setSamples(samples);
					EndDialog(hwnd, wParam);
					*w = false;
					return TRUE;
					// ---------------------------------------IDB_BACKGROUND ----------------------------
				case IDB_BACKGND:
				{
					auto &color = sett.BackGroundColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// --------------------------IDB_AXIS ----------------------------------------------
				case IDB_AXIS:
				{
					auto &color = sett.AxisColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// --------------------------------IDB_BOTTOM -------------------------------------------
				case IDB_BOTTOM:
				{
					auto &color = sett.BottomColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// --------------------------------------------------IDB_MGRID-----------------------------
				case IDB_MGRID:
				{
					auto &color = sett.GridMainColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// -------------------------------------------IDB_SGRID ------------------------------------
				case IDB_SGRID:
				{
					auto &color = sett.GridSecondColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// --------------------------------------------IDB_SIDE -------------------------------------
				case IDB_SIDE:
				{
					auto &color = sett.SideColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// --------------------------------------------IDB_GRAPH -------------------------------------
				case IDB_GRAPH:
				{
					auto &color = sett.GraphColor;
					cc.rgbResult = color.ToCOLORREF();
					ChooseColor(&cc);
					color.SetFromCOLORREF(cc.rgbResult);
					InvalidateRect(hwnd, NULL, false);
					return TRUE;
				}
				// ----------------------------------IDE_GRAPH -----------------------------------------------
				case IDE_GRAPHW:
				{
					switch (HIWORD(wParam))
					{
						case EN_CHANGE:
						{
							char buf[10];
							GetWindowText(HE_Graph_W, buf, 10);
							sett.GraphWidth = atoi(buf);
							return TRUE;
						}
					}
					return FALSE;
				}
				// ----------------------------------------------IDE_GRAPHW ----------------------------------
				case IDE_AXISW:
				{
					switch (HIWORD(wParam))
					{
						case EN_CHANGE:
						{
							char buf[10];
							GetWindowText(HE_Axis_W, buf, 10);
							sett.AxisWidth = atoi(buf);
							return TRUE;
						}
					}
					return FALSE;
				}
				// ----------------------------------------IDCB_WF ------------------------------------------
				case IDCB_WF:
				{
					switch (HIWORD(wParam))
					{
						case CBN_SELCHANGE:
						{
							int index = (int)SendMessage(HCB_WF, CB_GETCURSEL, 0, 0L);
							if (com)
								com->setWindowFunction(ChooseWF(index));
							return TRUE;
						}
					}
					return FALSE;
				}
				// -------------------------------------------IDE_SAMPLE ---------------------------------------
				case IDE_SAMPLE:
				{
					switch (HIWORD(wParam))
					{
						case EN_CHANGE:
						{
							char buf[10];
							GetWindowText(HE_Sample, buf, 10);
							samples = atoi(buf);
							//		com->setSamples(n);
							return TRUE;
						}
					}
					return FALSE;
				}
			}
		}
		// -------------------------------------WM_CTLCOLORBUTTON ---------------------------------------
		case WM_CTLCOLORBTN:
		{
			Color color;
			if ((HWND)lParam == HPB_Background)
			{
				color = sett.BackGroundColor;
			}
			else if ((HWND)lParam == HPB_Bottom)
			{
				color = sett.BottomColor;
			}
			else if ((HWND)lParam == HPB_Side)
			{
				color = sett.SideColor;
			}
			else if ((HWND)lParam == HPB_Axis)
			{
				color = sett.AxisColor;
			}
			else if ((HWND)lParam == HPB_GridM)
			{
				color = sett.GridMainColor;
			}
			else if ((HWND)lParam == HPB_GridS)
			{
				color = sett.GridSecondColor;
			}
			else if ((HWND)lParam == HPB_Graph)
			{
				color = sett.GraphColor;
			}
			else
			{
				return FALSE;
			}
			SetBkColor((HDC)wParam, color.ToCOLORREF());
			return (LONG)CreateSolidBrush(color.ToCOLORREF());
		}
	}
	return FALSE;
}
