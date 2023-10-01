#pragma once
#include <gdiplus.h>

struct ColorsGraph
{
	using C = Gdiplus::Color;

	C BackGroundColor;
	C SideColor;
	C AxisColor;
	C GridMainColor;
	C GridSecondColor;
	C GraphColor;
	C BottomColor;
	float GraphWidth;
	float AxisWidth;
};
