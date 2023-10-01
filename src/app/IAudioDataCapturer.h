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
	virtual std::vector<std::string> GetDeviceList() = 0;

	// Выбирает устройство по индексу вектора из функции GetDeviceList
	virtual void PickDevice(unsigned index) = 0;

	// Изменяет выбранное устройство захвата
	virtual void ChangeDevice(unsigned index) = 0;

	// Инициализирует модуль
	virtual void Initialize(size_t buffer_size) = 0;

	// Начинает захват данных
	virtual void Start() = 0;

	// Останавливает захват данных
	virtual void Stop() = 0;

	// Освобождает все ресурсы
	virtual void Release() = 0;

	// Возвращает частоту дискретизации
	virtual UINT32 GetSampleFrenq() = 0;

	// Возвращает true, если захват идет
	virtual bool isRun() = 0;

	// Изменяет размер буфера
	virtual void SetNSamples(int n) = 0;
};

// Фабрика, для создания экземпляра интерфейса
class FactoryCapturer
{
public:
	static IAudioDataCapturer *make();
};
