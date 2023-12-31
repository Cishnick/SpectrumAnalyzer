﻿#include "stdafx.h"

#include <Windows.h>

#include "src/gui/Application.h"

namespace
{
constexpr auto BUFFER_SIZE = 4800;
constexpr auto WINDOW_WIDTH = 1250;
constexpr auto WINDOW_HEIGHT = 493;
constexpr auto GRAPH_WIDTH = 913;
constexpr auto GRAPH_HEIGHT = 420;
} // namespace

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::unique_ptr<Application> application;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	application = std::make_unique<Application>(BUFFER_SIZE, WINDOW_WIDTH, WINDOW_HEIGHT, GRAPH_WIDTH, GRAPH_HEIGHT);
	application->initialize("Спектроанализатор", wndProc, hInst, nCmdShow);

	application->run();

	CoUninitialize();
	return 0;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY: PostQuitMessage(0); return 0;
		case WM_CREATE: application->createHandler(hWnd); return 0;
		case WM_PAINT: application->paintHandler(hWnd); return 0;
		case WM_SIZE: application->sizeHandler(LOWORD(lParam), HIWORD(lParam)); return 0;
		case WM_COMMAND: application->commandHandler(lParam, wParam); return 0;
		default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}