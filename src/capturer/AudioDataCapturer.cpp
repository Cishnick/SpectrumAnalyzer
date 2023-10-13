#include "src/app/stdafx.h"

#include "AudioDataCapturer.h"

#include "AudioDevicesGetter/audio_devices_getter.h"

std::unique_ptr<IAudioDataCapturer> FactoryCapturer::make()
{
	return std::make_unique<AudioDataCapturer>();
}

namespace
{
constexpr auto kDefaultBufferSize = 4800;
}

AudioDataCapturer::AudioDataCapturer()
	: bufferSize(0)
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

	bufferRightChannel = RingBuffer<double>(fifo_size);
	bufferLeftChannel = RingBuffer<double>(fifo_size);
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
	bool wasRunning = running;
	stop();

	currentDeviceIndex = index;
	bufferRightChannel.reset();
	bufferLeftChannel.reset();

	audioDevicesGetter->update();
	initialize(bufferRightChannel.getSize());

	if (wasRunning)
		start();
}

UINT32 AudioDataCapturer::getSampleFrenq()
{
	return mixFormat->nSamplesPerSec;
}

void AudioDataCapturer::run()
{
	UINT32 packetSize = 0;
	BYTE *pData;
	DWORD flags;
	std::pair<float, float> rawData;

	const auto actualPacketDuration = static_cast<double>(bufferSize) / mixFormat->nSamplesPerSec;

	while (running)
	{
		while (packetSize == 0)
		{
			if (auto hr = captureClient->GetNextPacketSize(&packetSize); FAILED(hr))
				throw std::exception("Unable to get next packet size", hr);
			Sleep(static_cast<unsigned>(actualPacketDuration) / 2);
		}

		if (auto hr = captureClient->GetBuffer(&pData, &bufferSize, &flags, nullptr, nullptr); FAILED(hr))
			throw std::exception("Unable to get buffer", hr);

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			for (size_t i = 0; i < bufferSize; i++)
			{
				auto rightChannel = bufferRightChannel.pushBack(0.);
				auto leftChannel = bufferLeftChannel.pushBack(0.);

				if (rightChannel)
					updateData(*rightChannel, *leftChannel);
			}
		else
			for (size_t i = 0; i < bufferSize * 2; i++)
			{
				CopyMemory(&rawData.first, reinterpret_cast<float *>(pData) + i++, sizeof(float));
				CopyMemory(&rawData.second, reinterpret_cast<float *>(pData) + i, sizeof(float));
				auto rightChannel = bufferRightChannel.pushBack(static_cast<double>(rawData.first));
				auto leftChannel = bufferLeftChannel.pushBack(static_cast<double>(rawData.second));

				if (rightChannel)
					updateData(*rightChannel, *leftChannel);
			}

		if (auto hr = captureClient->ReleaseBuffer(bufferSize); FAILED(hr))
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

AudioDataCapturerObservable &AudioDataCapturer::observable()
{
	return observable_;
}

void AudioDataCapturer::updateData(std::span<double> rightChannel, std::span<double> leftChannel)
{
	auto update = observable_.update();
	update->leftChannel = leftChannel;
	update->rightChannel = rightChannel;
	update->sampleFrequency = mixFormat->nSamplesPerSec;
}
