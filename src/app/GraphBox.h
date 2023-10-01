#pragma once
#include "stdafx.h"

#include <gdiplus.h>

#include <tuple>

#include "IObserver.h"
#include "SettStructs.h"

#define K 1e3

class GraphBox : public IObserverGraph
{
public:
	GraphBox(Gdiplus::Rect rect, size_t buffer_size, double f_step, double ampl);

	void Resize(Gdiplus::Rect rect);

	void Rendering(HDC hdc);

	void SetData(Reals &const R);

	void SetXMin(double new_xmin);
	void SetXMax(double new_xmax);
	void SetXSplit(int new_xstep);

	void SetYMin(double new_ymin);
	void SetYMax(double new_ymax);
	void SetYSplit(int new_ystep);

	void SetColors(ColorsGraph const &sett) override;

	void SetSampleN(int n, double f_step);

	std::pair<double, double> SetLogX(bool state);
	std::pair<double, double> SetLogY(bool state);

	~GraphBox();

private:
	void DrawGrid(Gdiplus::Graphics &g);

	std::wstring HztoStr(double hz);

	std::wstring AmpltoStr(double a);

	Gdiplus::Color _BackgroundColor;
	Gdiplus::Color _GridMainColor;
	Gdiplus::Color _GridSecColor;
	Gdiplus::Color _AxisColor;
	Gdiplus::Color _GraphicColor;
	Gdiplus::Color _BottomColor;
	Gdiplus::Color _RightColor;
	Gdiplus::Color _LeftColor;
	Gdiplus::Rect _PaintRect;
	Gdiplus::Rect _BottomRect;
	Gdiplus::Rect _LeftRect;
	Gdiplus::Rect _RightRect;
	Gdiplus::Rect _MainRect;

	float _GraphicWidth;
	float _AxisWidth;
	const int _CSideWidth = 40;
	const int _CBottomHeight = 25;

	int _XMaxSplits = 65;
	int _YMaxSplits = 30;

	const double _YMinDB = -100;
	const double _YMinDB_def = -60;

	const double _YMaxDB = 0;
	const double _YMaxDB_def = -6;

	Gdiplus::PointF *_Data;
	size_t _Size;
	double _FrenqStep;
	double _MaxFrenq;
	double _MaxAmpl;

	double _XMin;
	double _XMax;
	int _XSplit;

	double _YMin;
	double _YMax;
	int _YSplit;

	size_t _DataSize;

	bool _isLogX;
	bool _isLogY;
};
