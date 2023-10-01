#include "src/app/stdafx.h"

#include "AudioDataCapturer.h"

#include "AudioDevicesGetter/audio_devices_getter.h"

IAudioDataCapturer *FactoryCapturer::make()
{
	return new AudioDataCapturer();
}

// -------------------------------------------Конструктор------------------------------------
AudioDataCapturer::AudioDataCapturer()
	: _device(nullptr)
	, _role(ERole::eMultimedia)
	, _buffer_r(0)
	, _buffer_l(0)
	, _AudioClient(NULL)
	, _CaptureClient(NULL)
	, _MixFormat(NULL)
	, _BufferSize(0)
	, _thread()
	, _isWorked(false)
	, audioDevicesGetter_(std::make_unique<AudioDevicesGetter>())
{
}

// -------------------------------------------------GetDeviceList----------------------------

VectorOfString AudioDataCapturer::GetDeviceList()
{
	return audioDevicesGetter_->getDevicesList();
}

// ------------------------------------------------PickDevice--------------------------------

void AudioDataCapturer::PickDevice(unsigned index)
{
	HRESULT hr;
	if (index >= audioDevicesGetter_->getDevicesNumber())
	{
		ErrorMessage("Wrong device index", index);
		return;
	}
	hr = audioDevicesGetter_->getDeviceCollection().Item(index, &_device);
	if (FAILED(hr))
	{
		ErrorMessage("Unable to retrieve device", hr);
	}
}

// ----------------------------------------------------------add-----------------------------

void AudioDataCapturer::add(IObserverAudio *obs)
{
	_observers.push_back(obs);
}

// ----------------------------------------------------remove--------------------------------

void AudioDataCapturer::remove(IObserverAudio *obs)
{
	_observers.remove(obs);
}

// ------------------------------------------------------Handle------------------------------

void AudioDataCapturer::Handle(Reals const &r_chan, Reals const &l_chan, unsigned sample_frenq)
{
	for (auto i : _observers)
	{
		i->Buffer_OVF(r_chan, l_chan);
	}
}

// -------------------------------------------initialize-------------------------------------

void AudioDataCapturer::Initialize(size_t buffer_size)
{
	HRESULT hr;
	hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void **>(&_AudioClient));
	if (FAILED(hr))
	{
		ErrorMessage("Unable device activation", hr);
	}

	hr = _AudioClient->GetMixFormat(&_MixFormat);
	if (FAILED(hr))
	{
		ErrorMessage("Unable get mix format", hr);
	}

	hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 20 * static_cast<UINT64>(10000), 0, _MixFormat, NULL);
	if (FAILED(hr))
	{
		ErrorMessage("Unable initialize audio client", hr);
	}

	hr = _AudioClient->GetBufferSize(&_BufferSize);
	if (FAILED(hr))
	{
		ErrorMessage("Unable to get buffer size", hr);
	}

	hr = _AudioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void **>(&_CaptureClient));
	if (FAILED(hr))
	{
		ErrorMessage("Unable to get capture client", hr);
	}

	_buffer_r = RingBuffer<double>(buffer_size);
	_buffer_l = RingBuffer<double>(buffer_size);
}

// ------------------------------------------------------Start-------------------------------

void AudioDataCapturer::Start()
{
	HRESULT hr;

	_isWorked = true;
	_thread = std::thread([this]() { this->run(); });

	hr = _AudioClient->Start();
	if (FAILED(hr))
	{
		ErrorMessage("Unable to start audio client", hr);
	}
}

// --------------------------------------------Stop------------------------------------------

void AudioDataCapturer::Stop()
{
	_isWorked = false;
	_thread.join();
	_AudioClient->Stop();
}

// -------------------------------------------------Release----------------------------------

void AudioDataCapturer::Release()
{
	SafeRelease(&_device);
	SafeRelease(&_AudioClient);
	SafeRelease(&_CaptureClient);
}

// -------------------------------------------------isRun-----------------------------------

bool AudioDataCapturer::isRun()
{
	return _isWorked;
}

// ------------------------------------------SetNSamples------------------------------------

void AudioDataCapturer::SetNSamples(int n)
{
	bool temp = _isWorked;
	if (temp)
		Stop();
	Release();

	audioDevicesGetter_->update();
	PickDevice(_ind);
	Initialize(n);
	if (temp)
		Start();
}

// --------------------------------------------changeDevice----------------------------------

void AudioDataCapturer::ChangeDevice(unsigned index)
{
	bool temp = _isWorked;
	if (temp)
		Stop();
	Release();
	_buffer_r.Reset();
	_buffer_l.Reset();

	audioDevicesGetter_->update();
	PickDevice(index);
	Initialize(_buffer_r.getSize());
	if (temp)
		Start();

	_ind = index;
}

UINT32 AudioDataCapturer::GetSampleFrenq()
{
	return _MixFormat->nSamplesPerSec;
}

void AudioDataCapturer::run()
{
	UINT32 packetSize = 0;
	HRESULT hr;
	BYTE *pData;
	DWORD flags;
	float f[2];

	Sleep(2);

	while (_isWorked)
	{
		do
		{
			hr = _CaptureClient->GetNextPacketSize(&packetSize);
			if (FAILED(hr))
			{
				ErrorMessage("Unable to get next packet size", hr);
			}
		} while (packetSize == 0);

		hr = _CaptureClient->GetBuffer(&pData, &_BufferSize, &flags, NULL, NULL);
		if (FAILED(hr))
		{
			ErrorMessage("Unable to get buffer", hr);
		}

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			for (size_t i = 0; i < _BufferSize; i++)
			{
				auto pr = _buffer_r.push_back(0.);
				auto pl = _buffer_l.push_back(0.);

				if (pr && pl)
				{
					Handle(*pr, *pl, _MixFormat->nSamplesPerSec);
				}
			}
		}
		else
		{
			for (size_t i = 0; i < _BufferSize * 2; i += 2)
			{
				CopyMemory(f + 0, reinterpret_cast<float *>(pData) + i, sizeof(float));
				CopyMemory(f + 1, reinterpret_cast<float *>(pData) + i, sizeof(float));
				auto pr = _buffer_r.push_back(static_cast<double>(f[0]));
				auto pl = _buffer_l.push_back(static_cast<double>(f[1]));

				if (pr && pl)
				{
					Handle(*pr, *pl, _MixFormat->nSamplesPerSec);
				}
			}
		}
		hr = _CaptureClient->ReleaseBuffer(_BufferSize);
		if (FAILED(hr))
		{
			ErrorMessage("Unable to release buffer of capture client", hr);
		}
	}
}

AudioDataCapturer::~AudioDataCapturer() = default;
