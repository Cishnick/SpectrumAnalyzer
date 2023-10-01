#include "stdafx.h"

#include "GraphBox.h"
using namespace std;
using namespace Gdiplus;

// ----------------------------------Constructor -------------------------------------------------------
GraphBox::GraphBox(Gdiplus::Rect rect, size_t buffer_size, double f_step, double ampl)
	: _PaintRect(rect)
	, _Size(buffer_size)
	, _BackgroundColor(Color::Aqua)
	, _GridMainColor(Color::DarkGray)
	, _AxisColor(Color::Black)
	, _GraphicColor(Color::DarkBlue)
	, _Data(NULL)
	, _FrenqStep(f_step)
	, _MaxAmpl(ampl)
	, _MaxFrenq(f_step * buffer_size)
	, _BottomRect()
	, _LeftRect()
	, _RightRect()
	, _BottomColor(Color::LightGreen)
	, _RightColor(Color::LightBlue)
	, _LeftColor(Color::LightBlue)
	, _XMin(0)
	, _XMax(f_step * buffer_size)
	, _XSplit(_XMax / 10)
	, _YMin(0)
	, _YMax(ampl * K)
	, _YSplit(_YMax / 10)
	, _DataSize(buffer_size)
	, _GraphicWidth(2)
	, _isLogX(false)
	, _isLogY(false)
{
	_Data = new PointF[_Size];
}

// ----------------------------------------------Resize Paint Rectangle ------------------------------------
void GraphBox::Resize(Gdiplus::Rect rect)
{
	_MainRect = rect;
	_PaintRect = Rect(_CSideWidth, 0, rect.Width - 2 * _CSideWidth, rect.Height - _CBottomHeight);
	_BottomRect = Rect(0, rect.Height - _CBottomHeight, rect.Width, _CBottomHeight);
	_RightRect = Rect(rect.Width - _CSideWidth, 0, _CSideWidth, rect.Height - _CBottomHeight);
	_LeftRect = Rect(0, 0, _CSideWidth, rect.Height - _CBottomHeight);
}

// --------------------------------------------------Rendering ---------------------------------------
void GraphBox::Rendering(HDC hdc)
{
	Graphics GMain(hdc);
	Bitmap bmp(_MainRect.Width, _MainRect.Height, &GMain);
	Bitmap bmp2(_PaintRect.Width, _PaintRect.Height);
	Graphics GBuffer(&bmp), GPaintRect(&bmp2);
	SolidBrush b(_BackgroundColor);
	Pen p_axis(_AxisColor, _AxisWidth);

	GBuffer.FillRectangle(&b, _PaintRect);
	SolidBrush bot(_BottomColor);
	GBuffer.FillRectangle(&bot, _BottomRect);
	SolidBrush rig(_RightColor);
	GBuffer.FillRectangle(&rig, _RightRect);
	SolidBrush left(_LeftColor);
	GBuffer.FillRectangle(&left, _LeftRect);

	DrawGrid(GBuffer);

	GBuffer.DrawRectangle(&p_axis, _PaintRect);
	GBuffer.DrawRectangle(&p_axis, _BottomRect);
	GBuffer.DrawRectangle(&p_axis, _LeftRect);
	GBuffer.DrawRectangle(&p_axis, _RightRect);
	Pen penGraphic(_GraphicColor, _GraphicWidth);
	GPaintRect.DrawLines(&penGraphic, _Data, (INT)_DataSize);
	GBuffer.DrawImage(&bmp2, _PaintRect);
	GMain.DrawImage(&bmp, _MainRect);
}

// ------------------------------------------------------SetData -------------------------------------------
void GraphBox::SetData(Reals &const R)
{
	int w = _PaintRect.Width;
	int h = _PaintRect.Height;
	_DataSize = static_cast<size_t>((_XMax - _XMin) / _FrenqStep);
	double pix_step = (double)w / _DataSize;
	int i_0 = static_cast<int>(_XMin / _FrenqStep);
	double kx = w / (log10(_XMax) - log10(_XMin));
	double ky = h / log10((_YMax - _YMin) / K);
	for (size_t i = 0; i < _DataSize; i++)
	{
		//-----------------LOG--------------------------------------
		if (_isLogX)
			if (i == 0)
				_Data[i].X = 0;
			else
				_Data[i].X = static_cast<REAL>((log10(_XMin + i * _FrenqStep) - log10(_XMin)) * kx);
		//---------------------------------------------------- Line ------------------------------------
		else
			_Data[i].X = static_cast<REAL>(i * pix_step);
		// ------------------------------------ LOG Y --------------------------------------------------
		if (_isLogY)
		{
			//			auto temp = -log10(_YMax / K) - log10(pow(R[i + i_0], 2) - _YMin / K) / (log10(_YMax / K) - log10(_YMin / K));// *h;
			auto temp = (20 * log10(R[i + i_0]) - _YMin) / (_YMax - _YMin);
			if (temp < 0)
				temp = 0;
			_Data[i].Y = static_cast<REAL>(_PaintRect.Height - temp * h);
		}
		// ----------------------------------------- Line Y -------------------------------------------
		else
		{
			_Data[i].Y = static_cast<REAL>(_PaintRect.Height - (pow(R[i + i_0], 2) - _YMin / K) / (_YMax - _YMin) * h * K);
		}

		if (_Data[i].Y > _PaintRect.Height)
			_Data[i].Y = _PaintRect.Height;
	}
}

// -----------------------------------------------------Set X Parametres ------------------------------------
void GraphBox::SetXMin(double new_xmin)
{
	if (new_xmin >= _XMax)
		return;
	_XMin = new_xmin;
}

void GraphBox::SetXMax(double new_xmax)
{
	if (new_xmax <= _XMin)
		return;
	_XMax = new_xmax;
}

void GraphBox::SetXSplit(int new_xstep)
{
	if (!new_xstep)
		_XSplit = 1;
	else if (new_xstep <= _XMaxSplits)
		_XSplit = new_xstep;
	else
		_XSplit = _XMaxSplits;
}

void GraphBox::SetYMin(double new_ymin)
{
	if (new_ymin >= _YMax)
		return;
	_YMin = new_ymin;
}

// -----------------------------------------------------Set Y Parametres ------------------------------
void GraphBox::SetYMax(double new_ymax)
{
	if (new_ymax <= _YMin)
		return;
	_YMax = new_ymax;
}

void GraphBox::SetYSplit(int new_ystep)
{
	if (!new_ystep)
		_YSplit = 1;
	else if (new_ystep <= _YMaxSplits)
		_YSplit = new_ystep;
	else
		_YSplit = _YMaxSplits;
}

// ----------------------------------------------------SetColors --------------------------------------------
void GraphBox::SetColors(ColorsGraph const &sett)
{
	_BackgroundColor = sett.BackGroundColor;
	_GridMainColor = sett.GridMainColor;
	_GridSecColor = sett.GridSecondColor;
	_AxisColor = sett.AxisColor;
	_GraphicColor = sett.GraphColor;
	_BottomColor = sett.BottomColor;
	_RightColor = sett.SideColor;
	_LeftColor = sett.SideColor;
	_GraphicWidth = sett.GraphWidth;
	_AxisWidth = sett.AxisWidth;
}

void GraphBox::SetSampleN(int n, double f_step)
{
	_Size = n;
	_FrenqStep = f_step;
	_MaxFrenq = n * f_step;
	if (_XMax > _MaxFrenq)
		_XMax = _MaxFrenq;
	_DataSize = n;
	delete[] _Data;
	_Data = new PointF[_Size];
}

// ------------------------------------------------Set Log scale ----------------------------------------
std::pair<double, double> GraphBox::SetLogX(bool state)
{
	_isLogX = state;
	if (state)
	{
		if (_XMin == 0)
			_XMin = 1;
	}
	else
	{
		if (_XMin == 1)
			_XMin = 0;
	}
	return std::make_pair(_XMin, _XMax);
}

std::pair<double, double> GraphBox::SetLogY(bool state)
{
	_isLogY = state;
	if (state)
	{
		_YMin = _YMinDB_def;
		_YMax = _YMaxDB_def;
	}
	else
	{
		_YMin = 0;
		_YMax = 1e-1;
	}
	return std::make_pair(_YMin, _YMax);
}

// -------------------------------------Destructor ---------------------------------------------------
GraphBox::~GraphBox()
{
	delete[] _Data;
}

// ---------------------------------------------------DrawGrid-------------------------------------------
void GraphBox::DrawGrid(Graphics &g)
{
	//----------------------Pens---------------------------
	Pen penDash(_GridSecColor);
	penDash.SetDashStyle(DashStyleDash);
	Pen penSolid(_GridMainColor);
	//----------------------For DrawString--------------------
	SolidBrush brush(Color::Black);
	FontFamily family(L"Times New Roman");
	Font font(&family, 15, FontStyleRegular, UnitPixel);
	wchar_t str[10];
	// -----------------------замена переменных для удобства-----------
	double w = _PaintRect.Width;
	double h = _PaintRect.Height;

	wstring temp;

	//-----------------------Рисуем сетку по х для линейной оси-------------------------------
	if (!_isLogX)
	{
		//-------------------Это чтобы подписи не накладывались друг на друга
		int xs = static_cast<int>(60 / (w / _XSplit));
		for (size_t i = 0; i < _XSplit + 1; i++)
		{
			double x = i * w / _XSplit + _PaintRect.X;
			temp = HztoStr(i * (_XMax - _XMin) / _XSplit + _XMin);
			if (xs != 0)
				if (i % xs)
				{
					g.DrawLine(&penDash, Point(x, 0), Point(x, h));
					continue;
				}
			g.DrawLine(&penSolid, Point(x, 0), Point(x, h));
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - _PaintRect.X / 2 + 10, h), &brush);
		}
	}
	// ------------------------Рисуем сетку по Х для логарифмической оси-----------------------------------
	else
	{
		double x;
		double old_x = -50;
		double kx = w / (log10(_XMax) - log10(_XMin));
		for (double i = _XMin; i < _XMax + 1; i *= 10)
		{
			x = (log10(i) - log10(_XMin)) * kx + _PaintRect.X;
			g.DrawLine(&penSolid, Point(x, 0), Point(x, h));
			temp = HztoStr(i);
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - _PaintRect.X / 2 + 10, h), &brush);
			for (size_t j = 2 * i; j < 10 * i && j < _XMax + 1; j += i)
			{
				temp = HztoStr(j);
				x = (log10(j) - log10(_XMin)) * kx + _PaintRect.X;
				if (x - old_x > 25)
				{
					g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - _PaintRect.X / 2 + 10, h), &brush);
					old_x = x;
				}
				g.DrawLine(&penDash, Point(x, 0), Point(x, h));
			}
		}
	}
	// --------------------по У ось рисуется линейная и для лог. масштаба, т.к. децибелы располагаются линейно
	//-------------------Это чтобы подписи не накладывались друг на друга
	int ys = static_cast<int>(50 / (h / _YSplit));
	auto x = _RightRect.Width + _PaintRect.Width + 1;
	for (size_t i = 0; i < _YSplit; i++)
	{
		double y = h - i * h / _YSplit;
		temp = AmpltoStr(i * (_YMax - _YMin) / _YSplit + _YMin);
		if (ys != 0)
			if (i % ys)
			{
				g.DrawLine(&penDash, Point(0 + _PaintRect.X, y), Point(w + _PaintRect.X, y));
				continue;
			}
		g.DrawLine(&penSolid, Point(0 + _PaintRect.X, y), Point(w + _PaintRect.X, y));
		if (i)
		{
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(1, y - 7), &brush);
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x, y - 7), &brush);
		}
		else
		{
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(1, y - 16), &brush);
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x, y - 16), &brush);
		}
	}
	temp = AmpltoStr(_YMax);
	g.DrawString(temp.c_str(), temp.size(), &font, PointF(1, 0), &brush);
	g.DrawString(temp.c_str(), temp.size(), &font, PointF(x, 0), &brush);
}

// -----------------------------------------Hertz to String -----------------------------------------------------

std::wstring GraphBox::HztoStr(double hz)
{
	wstringstream stream;
	if (hz < 1000)
	{
		stream << static_cast<int>(hz);
	}
	else
	{
		if (hz < 10000.)
			stream << static_cast<double>(static_cast<int>(hz / 100)) / 10 << 'k';
		else
			stream << static_cast<int>(hz / 1000) << 'k';
	}
	stream << '\0';
	return stream.str();
}

// ---------------------------------Amplitude to String--------------------------------------------------
std::wstring GraphBox::AmpltoStr(double a)
{
	wstringstream stream;
	stream << a << "\0";
	return stream.str();
}
