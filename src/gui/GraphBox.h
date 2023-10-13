#pragma once
#include "src/app/stdafx.h"

#include <gdiplus.h>

#include <span>
#include <tuple>

#include "SettStructs.h"
#include "src/app/IObserver.h"

class GraphBox : public IObserverGraph
{
public:
	GraphBox(const Gdiplus::Rect &rect, size_t buffer_size, double f_step, double ampl);

	void resize(Gdiplus::Rect rect);

	void rendering(HDC hdc);

	void setData(std::span<double> data);

	void setXMin(double newXmin);
	void setXMax(double newXmax);
	void setXSplit(int newXstep);
	void setYMin(double newYmin);
	void setYMax(double newYmax);
	void setYSplit(int newYstep);

	void setColors(const struct ColorsGraph &sett) override;

	void setSampleN(int n, double f_step);

	std::pair<double, double> setLogX(bool state);
	std::pair<double, double> setLogY(bool state);

	~GraphBox() override;

private:
	void drawGrid(Gdiplus::Graphics &g);

	Gdiplus::Color backgroundColor;
	Gdiplus::Color gridMainColor;
	Gdiplus::Color gridSecColor;
	Gdiplus::Color axisColor;
	Gdiplus::Color graphicColor;
	Gdiplus::Color bottomColor;
	Gdiplus::Color rightColor;
	Gdiplus::Color leftColor;
	Gdiplus::Rect paintRect;
	Gdiplus::Rect bottomRect;
	Gdiplus::Rect leftRect;
	Gdiplus::Rect rightRect;
	Gdiplus::Rect mainRect;

	float graphicWidth;
	float axisWidth;

	int xMaxSplits;
	int yMaxSplits;

	Gdiplus::PointF *data_;
	size_t samplesSize;
	double freqStep;

	double xMin;
	double xMax;
	int xSplit;

	double yMin;
	double yMax;
	int ySplit;

	size_t dataSize;

	bool isLogX;
	bool isLogY;
};
