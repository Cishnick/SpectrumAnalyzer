#include "src/app/stdafx.h"

#include "GraphBox.h"
using namespace std;
using namespace Gdiplus;

namespace
{
constexpr double kYMinDefaultValue = -60;
constexpr double kYMaxDefaultValue = -6;

constexpr int cSideWidth = 40;
constexpr int cBottomHeight = 25;

constexpr auto kBackgroundColor = Color::Aqua;
constexpr auto kGridMainColor = Color::DarkGray;
constexpr auto kAxisColor = Color::Black;
constexpr auto kGraphicColor = Color::DarkBlue;
constexpr auto kBottomColor = Color::LightGreen;
constexpr auto kRightColor = Color::LightBlue;
constexpr auto kLeftColor = Color::LightBlue;

constexpr auto kXMaxSplits = 65;
constexpr auto kYMaxSplits = 30;

constexpr auto kMagnitudeSupression = 1e-3;

std::wstring hz2Str(double hz)
{
	wstringstream stream;
	if (hz < 1000)
		stream << static_cast<int>(hz);
	else if (hz < 10000.)
		stream << static_cast<double>(static_cast<int>(hz / 100)) / 10 << 'k';
	else
		stream << static_cast<int>(hz / 1000) << 'k';
	stream << '\0';
	return stream.str();
}

std::wstring ampl2Str(double a)
{
	wstringstream stream;
	stream << a;
	return stream.str();
}

} // namespace

GraphBox::GraphBox(const Gdiplus::Rect &rect, size_t buffer_size, double f_step, double ampl)
	: paintRect(rect)
	, dataSize(buffer_size)
	, backgroundColor(kBackgroundColor)
	, gridMainColor(kGridMainColor)
	, axisColor(kAxisColor)
	, graphicColor(kGraphicColor)
	, xMaxSplits(kXMaxSplits)
	, yMaxSplits(kYMaxSplits)
	, samplesSize(buffer_size)
	, freqStep(f_step)
	, bottomColor(kBottomColor)
	, rightColor(kRightColor)
	, leftColor(kLeftColor)
	, xMin(0)
	, xMax(f_step * buffer_size)
	, xSplit(static_cast<int>(xMax / 10))
	, yMin(0)
	, yMax(ampl * kMagnitudeSupression)
	, ySplit(static_cast<int>(yMax / 10))
	, graphicWidth(2)
	, isLogX(false)
	, isLogY(false)
{
	data_ = new PointF[samplesSize];
}

void GraphBox::resize(Gdiplus::Rect rect)
{
	mainRect = rect;
	paintRect = Rect(cSideWidth, 0, rect.Width - 2 * cSideWidth, rect.Height - cBottomHeight);
	bottomRect = Rect(0, rect.Height - cBottomHeight, rect.Width, cBottomHeight);
	rightRect = Rect(rect.Width - cSideWidth, 0, cSideWidth, rect.Height - cBottomHeight);
	leftRect = Rect(0, 0, cSideWidth, rect.Height - cBottomHeight);
}

void GraphBox::rendering(HDC hdc)
{
	Graphics GMain(hdc);
	Bitmap bmp(mainRect.Width, mainRect.Height, &GMain);
	Bitmap bmp2(paintRect.Width, paintRect.Height);
	Graphics GBuffer(&bmp), GPaintRect(&bmp2);
	SolidBrush b(backgroundColor);
	Pen p_axis(axisColor, axisWidth);

	GBuffer.FillRectangle(&b, paintRect);
	SolidBrush bot(bottomColor);
	GBuffer.FillRectangle(&bot, bottomRect);
	SolidBrush rig(rightColor);
	GBuffer.FillRectangle(&rig, rightRect);
	SolidBrush left(leftColor);
	GBuffer.FillRectangle(&left, leftRect);

	drawGrid(GBuffer);

	GBuffer.DrawRectangle(&p_axis, paintRect);
	GBuffer.DrawRectangle(&p_axis, bottomRect);
	GBuffer.DrawRectangle(&p_axis, leftRect);
	GBuffer.DrawRectangle(&p_axis, rightRect);
	Pen penGraphic(graphicColor, graphicWidth);
	GPaintRect.DrawLines(&penGraphic, data_, (INT)dataSize);
	GBuffer.DrawImage(&bmp2, paintRect);
	GMain.DrawImage(&bmp, mainRect);
}

void GraphBox::setData(std::span<double> data)
{
	int w = paintRect.Width;
	int h = paintRect.Height;
	dataSize = static_cast<size_t>((xMax - xMin) / freqStep);
	double pix_step = (double)w / dataSize;
	int i_0 = static_cast<int>(xMin / freqStep);
	double kx = w / (log10(xMax) - log10(xMin));
	for (size_t i = 0; i < dataSize; i++)
	{
		//-----------------LOG--------------------------------------
		if (isLogX)
			if (i == 0)
				data_[i].X = 0;
			else
				data_[i].X = static_cast<REAL>((log10(xMin + i * freqStep) - log10(xMin)) * kx);
		//---------------------------------------------------- Line ------------------------------------
		else
			data_[i].X = static_cast<REAL>(i * pix_step);
		// ------------------------------------ LOG Y --------------------------------------------------
		if (isLogY)
		{
			auto temp = (20 * log10(data[i + i_0]) - yMin) / (yMax - yMin);
			if (temp < 0)
				temp = 0;
			data_[i].Y = static_cast<REAL>(paintRect.Height - temp * h);
		}
		// ----------------------------------------- Line Y -------------------------------------------
		else
		{
			data_[i].Y = static_cast<REAL>(paintRect.Height
			                               - (pow(data[i + i_0], 2) - yMin / kMagnitudeSupression) / (yMax - yMin) * h
			                                     * kMagnitudeSupression);
		}

		if (data_[i].Y > paintRect.Height)
			data_[i].Y = paintRect.Height;
	}
}

void GraphBox::setXMin(double new_xmin)
{
	if (new_xmin >= xMax)
		return;
	xMin = new_xmin;
}

void GraphBox::setXMax(double new_xmax)
{
	if (new_xmax <= xMin)
		return;
	xMax = new_xmax;
}

void GraphBox::setXSplit(int new_xstep)
{
	if (!new_xstep)
		xSplit = 1;
	else if (new_xstep <= xMaxSplits)
		xSplit = new_xstep;
	else
		xSplit = xMaxSplits;
}

void GraphBox::setYMin(double new_ymin)
{
	if (new_ymin >= yMax)
		return;
	yMin = new_ymin;
}

void GraphBox::setYMax(double new_ymax)
{
	if (new_ymax <= yMin)
		return;
	yMax = new_ymax;
}

void GraphBox::setYSplit(int new_ystep)
{
	if (!new_ystep)
		ySplit = 1;
	else if (new_ystep <= yMaxSplits)
		ySplit = new_ystep;
	else
		ySplit = yMaxSplits;
}

void GraphBox::setColors(const struct ColorsGraph &sett)
{
	backgroundColor = sett.backGroundColor;
	gridMainColor = sett.gridMainColor;
	gridSecColor = sett.gridSecondColor;
	axisColor = sett.axisColor;
	graphicColor = sett.graphColor;
	bottomColor = sett.bottomColor;
	rightColor = sett.sideColor;
	leftColor = sett.sideColor;
	graphicWidth = sett.graphWidth;
	axisWidth = sett.axisWidth;
}

void GraphBox::setSampleN(int n, double f_step)
{
	samplesSize = n;
	freqStep = f_step;
	auto maxFreq = n * f_step;
	if (xMax > maxFreq)
		xMax = maxFreq;
	dataSize = n;
	delete[] data_;
	data_ = new PointF[samplesSize];
}

std::pair<double, double> GraphBox::setLogX(bool state)
{
	isLogX = state;
	if (state)
	{
		if (xMin == 0)
			xMin = 1;
	}
	else
	{
		if (xMin == 1)
			xMin = 0;
	}
	return std::make_pair(xMin, xMax);
}

std::pair<double, double> GraphBox::setLogY(bool state)
{
	isLogY = state;
	if (state)
	{
		yMin = kYMinDefaultValue;
		yMax = kYMaxDefaultValue;
	}
	else
	{
		yMin = 0;
		yMax = 1e-1;
	}
	return std::make_pair(yMin, yMax);
}

GraphBox::~GraphBox()
{
	delete[] data_;
}

void GraphBox::drawGrid(Gdiplus::Graphics &g)
{
	//----------------------Pens---------------------------
	Pen penDash(gridSecColor);
	penDash.SetDashStyle(DashStyleDash);
	Pen penSolid(gridMainColor);
	//----------------------For DrawString--------------------
	SolidBrush brush(Color::Black);
	FontFamily family(L"Times New Roman");
	Font font(&family, 15, FontStyleRegular, UnitPixel);
	wchar_t str[10];
	// -----------------------замена переменных для удобства-----------
	double w = paintRect.Width;
	double h = paintRect.Height;

	wstring temp;

	//-----------------------Рисуем сетку по х для линейной оси-------------------------------
	if (!isLogX)
	{
		//-------------------Это чтобы подписи не накладывались друг на друга
		int xs = static_cast<int>(60 / (w / xSplit));
		for (int i = 0; i < xSplit + 1; i++)
		{
			double x = i * w / xSplit + paintRect.X;
			temp = hz2Str(i * (xMax - xMin) / xSplit + xMin);
			if (xs != 0)
				if (i % xs)
				{
					g.DrawLine(&penDash, Point(x, 0), Point(x, h));
					continue;
				}
			g.DrawLine(&penSolid, Point(x, 0), Point(x, h));
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - paintRect.X / 2 + 10, h), &brush);
		}
	}
	// ------------------------Рисуем сетку по Х для логарифмической оси-----------------------------------
	else
	{
		double x;
		double old_x = -50;
		double kx = w / (log10(xMax) - log10(xMin));
		for (double i = xMin; i < xMax + 1; i *= 10)
		{
			x = (log10(i) - log10(xMin)) * kx + paintRect.X;
			g.DrawLine(&penSolid, Point(x, 0), Point(x, h));
			temp = hz2Str(i);
			g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - paintRect.X / 2 + 10, h), &brush);
			for (size_t j = 2 * i; j < 10 * i && j < xMax + 1; j += i)
			{
				temp = hz2Str(j);
				x = (log10(j) - log10(xMin)) * kx + paintRect.X;
				if (x - old_x > 25)
				{
					g.DrawString(temp.c_str(), temp.size(), &font, PointF(x - paintRect.X / 2 + 10, h), &brush);
					old_x = x;
				}
				g.DrawLine(&penDash, Point(x, 0), Point(x, h));
			}
		}
	}
	// --------------------по У ось рисуется линейная и для лог. масштаба, т.к. децибелы располагаются линейно
	//-------------------Это чтобы подписи не накладывались друг на друга
	int ys = static_cast<int>(50 / (h / ySplit));
	auto x = rightRect.Width + paintRect.Width + 1;
	for (size_t i = 0; i < ySplit; i++)
	{
		double y = h - i * h / ySplit;
		temp = ampl2Str(i * (yMax - yMin) / ySplit + yMin);
		if (ys != 0)
			if (i % ys)
			{
				g.DrawLine(&penDash, Point(0 + paintRect.X, y), Point(w + paintRect.X, y));
				continue;
			}
		g.DrawLine(&penSolid, Point(0 + paintRect.X, y), Point(w + paintRect.X, y));
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
	temp = ampl2Str(yMax);
	g.DrawString(temp.c_str(), temp.size(), &font, PointF(1, 0), &brush);
	g.DrawString(temp.c_str(), temp.size(), &font, PointF(x, 0), &brush);
}
