#include "stdafx.h"

#include "Application.h"

#include "src/capturer/AudioDataCapturer.h"


Application::Application(size_t NumOfCounts, int window_width, int window_height, int graph_width, int graph_height)
	: _Capturer(NULL)
	, _Analyzer(NULL)
	, _Gui(NULL)
	, _NumOfCounts(NumOfCounts)
	, _WindWidth(window_width)
	, _WindHeight(window_height)
	, _GraphWidth(graph_width)
	, _GraphHeight(graph_height)
{
	_Capturer = FactoryCapturer::make();
	_Analyzer = FactoryAnalyzer::make(_Capturer->GetSampleFrenq());
	_Analyzer->Initialize(_NumOfCounts, ChannelsMode::Right, WF::BlackmanNuttall, GraphMode::Amplitude);
	_Gui = GUIFactory::make(_NumOfCounts, _Analyzer->FrenqStep(), _WindWidth, _WindHeight);
}

void Application::initialize(LPCSTR HeaderWindow, WNDPROC wproc, HINSTANCE hInst, int cmd)
{
	_Analyzer->add(_Gui);
	_Capturer->add(_Analyzer);
	_Gui->add(*this);
	_Gui->Initialize(std::string(HeaderWindow), wproc, hInst, cmd);
	_Gui->setDevList(_Capturer->getDeviceList());
	_Gui->connectCom(this);
}

void Application::createHandler(HWND hWnd)
{
	_Gui->CreateHandler(_GraphWidth, _GraphHeight, 7, 7, hWnd);
}

void Application::paintHandler(HWND hWnd)
{
	_Gui->Rendering(hWnd);
}

void Application::sizeHandler(int new_width, int hew_height)
{
	_Gui->ChangeSize(new_width, hew_height);
}

void Application::commandHandler(HWND hWnd, LPARAM lParam, WPARAM wParam)
{
	_Gui->CommandHandler(wParam, lParam);
}

int Application::run()
{
	MSG msg;
	while (GetMessage(&msg, static_cast<HWND>(NULL), 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (_Capturer->isRunning())
		_Capturer->stop();
	_Analyzer->Release();
	_Gui->Release();
	return 0;
}

void Application::Switch(bool is_turnon)
{
	if (is_turnon)
	{
		_Capturer->start();
	}
	else
	{
		_Capturer->stop();
	}
}

void Application::changeDevice(int index)
{
	_Capturer->changeDevice(index);
}

void Application::setWindowFunction(fd_t func)
{
	_Analyzer->SetWindowFunc(func);
}

void Application::setSamples(int n_sample)
{
	_NumOfCounts = n_sample;
	_Capturer->setFifoSize(n_sample);
	_Analyzer->ChangePacketSize(n_sample);
	_Gui->SetPacketSize(n_sample, _Analyzer->FrenqStep());
}

Application::~Application()
{
	delete _Capturer;
	delete _Analyzer;
	delete _Gui;
}
