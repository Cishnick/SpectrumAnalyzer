#pragma once
#include <gdiplus.h>

struct ColorsGraph
{
	using Color = Gdiplus::Color;

	Color backGroundColor;
	Color sideColor;
	Color axisColor;
	Color gridMainColor;
	Color gridSecondColor;
	Color graphColor;
	Color bottomColor;
	double graphWidth;
	double axisWidth;
};
