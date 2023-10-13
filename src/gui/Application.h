#pragma once
#include "src/app/stdafx.h"

#include "IAudioDataCapturer.h"
#include "IGUI.h"
#include "src/app/IObserver.h"
#include "src/app/ISpectrumAnalyser.h"
#include "src/app/WindowFunc.h"
#include "src/interface/observer/observer_controller.h"
#include "src/interface/raii/propagate_const_ptr.h"

class Application : public IObserverCommand
{
public:
	Application(size_t numOfCounts, int windowWidth, int windowHeight, int graphWidth, int graphHeight);

	void initialize(LPCSTR headerWindow, WNDPROC wproc, HINSTANCE hInst, int cmd);

	void createHandler(HWND hWnd);

	void paintHandler(HWND hWnd);

	void sizeHandler(int newWidth, int newHeight);

	void commandHandler(LPARAM lParam, WPARAM wParam);

	int run();

	void switchCapturing(bool enable) override;

	void changeDevice(int index) override;

	void setWindowFunction(WindowFunctor func) override;

	void setSamples(int n_sample) override;

	~Application() override;

private:
	PropagateConstPtr<ObserverController> observerController;
	PropagateConstPtr<IAudioDataCapturer> capturer;
	PropagateConstPtr<ISpectrumAnalyser> analyzer;

	// Todo: change to PropagateConst (problem with initialize)
	std::unique_ptr<IGUI> gui;
	int graphWidth;
	int graphHeight;
};
