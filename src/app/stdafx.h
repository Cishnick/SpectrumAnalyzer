#pragma once

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

using VectorOfString = std::vector<std::string>;
using Reals = std::vector<double>;
using WindowFunctor = std::function<double(size_t, size_t)>;
using std::string;
