#pragma once
#include "src/app/stdafx.h"

#include <Audioclient.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys.h>
#include <mmdeviceapi.h>

#include "src/app/IObserver.h"
#include "src/interface/observer/observable.h"

class IAudioDataCapturer
{
public:
	virtual struct AudioDataCapturerObservable& observable() = 0;

	virtual ~IAudioDataCapturer() = default;

	virtual std::vector<std::string> getDeviceList() = 0;

	virtual void changeDevice(unsigned index) = 0;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual UINT32 getSampleFrenq() = 0;

	virtual bool isRunning() = 0;

	virtual void setFifoSize(int n) = 0;
};

class FactoryCapturer
{
public:
	static std::unique_ptr<IAudioDataCapturer> make();
};
