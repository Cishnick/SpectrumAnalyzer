#pragma once
#include "stdafx.h"

#include "IAudioDataCapturer.h"
#include "IGUI.h"
#include "IObserver.h"
#include "ISpectrumAnalyser.h"
#include "WindowFunc.h"

class Application : public IObserverCommand
{
public:
	Application(size_t NumOfCounts, int window_width, int window_height, int graph_width, int graph_height);

	void initialize(LPCSTR HeaderWindow, WNDPROC wproc, HINSTANCE hInst, int cmd);

	void createHandler(HWND hWnd);

	void paintHandler(HWND hWnd);

	void sizeHandler(int new_width, int hew_height);

	void commandHandler(HWND hWnd, LPARAM lParam, WPARAM wParam);

	int run();

	void Switch(bool is_turnon) override;

	void changeDevice(int index) override;

	void setWindowFunction(fd_t func) override;

	void setSamples(int n_sample) override;

	~Application();

private:
	IAudioDataCapturer *_Capturer;
	ISpectrumAnalyser *_Analyzer;
	IGUI *_Gui;
	size_t _NumOfCounts;
	int _WindWidth;
	int _WindHeight;
	int _GraphWidth;
	int _GraphHeight;
};
