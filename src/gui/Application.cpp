#include "src/app/stdafx.h"

#include "Application.h"

#include "src/capturer/AudioDataCapturer.h"
#include "src/capturer/audio_data_capturer_observable.h"

Application::Application(size_t numOfCounts, int windowWidth, int windowHeight, int graphWidth, int graphHeight)
	: observerController(std::make_unique<ObserverController>())
	, capturer(FactoryCapturer::make())
	, analyzer(FactoryAnalyzer::make(capturer->getSampleFrenq()))
	, graphWidth(graphWidth)
	, graphHeight(graphHeight)
{
	analyzer->initialize(numOfCounts, WF::BlackmanNuttall);
	gui.reset(GUIFactory::make(numOfCounts, analyzer->freqStep(), windowWidth, windowHeight));
}

void Application::initialize(LPCSTR headerWindow, WNDPROC wproc, HINSTANCE hInst, int cmd)
{
	capturer->observable().register_observer(observerController.get(),
	                                         [this](const auto &observable)
	                                         {
												 // Todo: Now only for right channel implemented
												 auto magnitude = analyzer->calculate(observable.rightChannel);
												 gui->drawSpectr(magnitude);
											 });
	gui->add(*this);
	gui->initialize(std::string(headerWindow), wproc, hInst, cmd);
	gui->setDevList(capturer->getDeviceList());
	gui->connectCom(this);
}

void Application::createHandler(HWND hWnd)
{
	gui->createHandler(graphWidth, graphHeight, 7, 7, hWnd);
}

void Application::paintHandler(HWND hWnd)
{
	gui->rendering(hWnd);
}

void Application::sizeHandler(int newWidth, int hewHeight)
{
	gui->changeSize(newWidth, hewHeight);
}

void Application::commandHandler(LPARAM lParam, WPARAM wParam)
{
	gui->commandHandler(wParam, lParam);
}

int Application::run()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	capturer->stop();
	gui->release();
	return 0;
}

void Application::switchCapturing(bool enable)
{
	if (enable)
		capturer->start();
	else
		capturer->stop();
}

void Application::changeDevice(int index)
{
	capturer->changeDevice(index);
}

void Application::setWindowFunction(WindowFunctor func)
{
	analyzer->setWindowFunc(func);
}

void Application::setSamples(int n_sample)
{
	capturer->setFifoSize(n_sample);
	analyzer->changePacketSize(n_sample);
	gui->setPacketSize(n_sample, analyzer->freqStep());
}

Application::~Application() = default;
