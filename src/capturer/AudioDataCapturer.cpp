#include "src/app/stdafx.h"

#include "AudioDataCapturer.h"

#include <cassert>

#include "AudioDevicesGetter/audio_devices_getter.h"

constexpr auto kReftimesPerSec = 10000000;

IAudioDataCapturer *FactoryCapturer::make()
{
	return new AudioDataCapturer();
}

namespace
{
constexpr auto kDefaultBufferSize = 4800;
}

AudioDataCapturer::AudioDataCapturer()
	: _buffer_r(0)
	, _buffer_l(0)
	, bufferSize(0)
	, thread()
	, running(false)
	, currentDeviceIndex(0)
	, audioDevicesGetter(std::make_unique<AudioDevicesGetter>())
{
	initialize(kDefaultBufferSize);
}

std::vector<std::string> AudioDataCapturer::getDeviceList()
{
	return audioDevicesGetter->getDevicesList();
}

void AudioDataCapturer::add(IObserverAudio *obs)
{
	_observers.push_back(obs);
}

void AudioDataCapturer::remove(IObserverAudio *obs)
{
	_observers.remove(obs);
}

void AudioDataCapturer::Handle(Reals const &r_chan, Reals const &l_chan, unsigned sample_frenq)
{
	for (auto i : _observers)
	{
		i->Buffer_OVF(r_chan, l_chan);
	}
}

void AudioDataCapturer::initialize(size_t fifo_size)
{
	audioClient.reset(IAudioClientPtr(IMMDevicePtr(audioDevicesGetter->getDeviceCollection(), currentDeviceIndex), CLSCTX_ALL));
	mixFormat.reset(WaveFormatExPtr(audioClient));

	if (auto hr =
	        audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 20 * static_cast<UINT64>(10000), 0, mixFormat.get(), nullptr);
	    FAILED(hr))
		throw std::exception("Unable initialize audio client", hr);

	if (auto hr = audioClient->GetBufferSize(&bufferSize); FAILED(hr))
		throw std::exception("Unable to get buffer size");

	captureClient = IAudioCaptureClientPtr(audioClient);

	_buffer_r = RingBuffer<double>(fifo_size);
	_buffer_l = RingBuffer<double>(fifo_size);
}

void AudioDataCapturer::start()
{
	if (running)
		return;

	running = true;
	thread = std::thread(&AudioDataCapturer::run, this);

	if (auto hr = audioClient->Start(); FAILED(hr))
		throw std::exception("Unable to start audio client", hr);
}

void AudioDataCapturer::stop()
{
	stop_impl();
}

bool AudioDataCapturer::isRunning()
{
	return running;
}

void AudioDataCapturer::setFifoSize(int n)
{
	stop();
	audioDevicesGetter->update();
	initialize(n);
	start();
}

void AudioDataCapturer::changeDevice(unsigned index)
{
	currentDeviceIndex = index;

	stop();
	_buffer_r.Reset();
	_buffer_l.Reset();

	audioDevicesGetter->update();
	initialize(_buffer_r.getSize());

	start();
}

UINT32 AudioDataCapturer::GetSampleFrenq()
{
	return mixFormat->nSamplesPerSec;
}

void AudioDataCapturer::run()
{
	UINT32 packetSize = 0;
	HRESULT hr;
	BYTE *pData;
	DWORD flags;
	float f[2];

	const auto actualPacketDuration = static_cast<double>(bufferSize) / mixFormat->nSamplesPerSec;

	while (running)
	{
		while (packetSize == 0)
		{
			if (auto hr = captureClient->GetNextPacketSize(&packetSize); FAILED(hr))
				throw std::exception("Unable to get next packet size", hr);
			Sleep(static_cast<unsigned>(actualPacketDuration) / 2);
		}

		hr = captureClient->GetBuffer(&pData, &bufferSize, &flags, NULL, NULL);
		if (FAILED(hr))
			throw std::exception("Unable to get buffer", hr);

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			for (size_t i = 0; i < bufferSize; i++)
			{
				auto pr = _buffer_r.push_back(0.);
				auto pl = _buffer_l.push_back(0.);

				if (pr)
					Handle(*pr, *pl, mixFormat->nSamplesPerSec);
			}
		else
			for (size_t i = 0; i < bufferSize * 2; i += 2)
			{
				CopyMemory(f + 0, reinterpret_cast<float *>(pData) + i, sizeof(float));
				CopyMemory(f + 1, reinterpret_cast<float *>(pData) + i, sizeof(float));
				auto pr = _buffer_r.push_back(static_cast<double>(f[0]));
				auto pl = _buffer_l.push_back(static_cast<double>(f[1]));

				if (pr)
					Handle(*pr, *pl, mixFormat->nSamplesPerSec);
			}
		hr = captureClient->ReleaseBuffer(bufferSize);
		if (FAILED(hr))
			throw std::exception("Unable to release buffer of capture client", hr);
	}
}

AudioDataCapturer::~AudioDataCapturer()
{
	stop_impl();
}

void AudioDataCapturer::stop_impl()
{
	if (!running)
		return;

	running = false;
	thread.join();
	audioClient->Stop();
}
