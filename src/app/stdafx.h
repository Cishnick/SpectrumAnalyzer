#pragma once
#pragma warning(push)
#pragma warning(disable : 4717)
#pragma warning(disable : 4334)
#pragma warning(disable : 4227)

#pragma comment(lib, "Gdiplus.lib")

#include <algorithm>
#include <complex>
#include <ctime>
#include <functional>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

#include "resource.h"

#define ErrorMessageShow(str)              \
	{                                      \
		MessageBox(NULL, str, "Error", 0); \
	}

#define ErrorMessage(str, x)         \
	{                                \
		printf("%s : %x\n", str, x); \
		system("pause");             \
		exit(x);                     \
	}

template <class T>
void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

using VectorOfString = std::vector<std::string>;
using Reals = std::vector<double>;
using fd_t = std::function<double(size_t, size_t)>;
using std::string;

enum GraphMode
{
	Amplitude,
	Phase
};

enum ChannelsMode
{
	Right,
	Left,
	RightPlusLeft,
	RightMinusLeft,
	RightAndLeft
};
