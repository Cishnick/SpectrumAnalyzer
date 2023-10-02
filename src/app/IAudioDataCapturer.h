#pragma once
#include "stdafx.h"

#include <AudioClient.h>
#include <AudioPolicy.h>
#include <MMDeviceAPI.h>
#include <functiondiscoverykeys.h>

#include "IObserver.h"

/*
	IAudioDataCapturer - высокоуровневый интерфейс, предоставляющий доступ к управлению захватом данных
	с аудиопотока и к буферу, в котором хранятся эти данные.

*/

class IAudioDataCapturer
{
public:
	// Добавляет наблюдателя
	virtual void add(IObserverAudio *obs) = 0;

	// Удаляет наблюдателя
	virtual void remove(IObserverAudio *obs) = 0;

	// Возвращает набор строк с именами устройств захвата
	virtual std::vector<std::string> getDeviceList() = 0;

	// Изменяет выбранное устройство захвата
	virtual void changeDevice(unsigned index) = 0;

	// Начинает захват данных
	virtual void start() = 0;

	// Останавливает захват данных
	virtual void stop() = 0;

	// Возвращает частоту дискретизации
	virtual UINT32 GetSampleFrenq() = 0;

	// Возвращает true, если захват идет
	virtual bool isRunning() = 0;

	// Изменяет размер буфера
	virtual void setFifoSize(int n) = 0;
};

// Фабрика, для создания экземпляра интерфейса
class FactoryCapturer
{
public:
	static IAudioDataCapturer *make();
};
