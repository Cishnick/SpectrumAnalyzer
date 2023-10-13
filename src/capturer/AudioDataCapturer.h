#include "src/app/stdafx.h"

#include <atomic>
#include <thread>

#include "RingBuffer.h"
#include "raii/api_wrappers.h"
#include "src/gui/IAudioDataCapturer.h"
#include "audio_data_capturer_observable.h"

using namespace std;

// Основной класс, реализующий интерфейс
class AudioDataCapturer : public IAudioDataCapturer
{
public:
	AudioDataCapturer();
	~AudioDataCapturer() override;

	AudioDataCapturerObservable& observable() override;
	std::vector<std::string> getDeviceList() override;

	void changeDevice(unsigned index) override;

	UINT32 getSampleFrenq() override;

	void start() override;

	void stop() override;

	bool isRunning() override;

	void setFifoSize(int n) override;

private:
	void updateData(std::span<double> rightChannel, std::span<double> leftChannel);

	void initialize(size_t fifo_size);

	// For call from destructor instead virtual function 'stop'
	void stop_impl();

	void run();

private:
	IAudioClientPtr audioClient;
	IAudioCaptureClientPtr captureClient;
	WaveFormatExPtr mixFormat;
	UINT32 bufferSize;
	std::thread thread;
	std::atomic_bool running;
	RingBuffer<double> bufferRightChannel;
	RingBuffer<double> bufferLeftChannel;
	unsigned currentDeviceIndex;

private:
	std::unique_ptr<class AudioDevicesGetter> audioDevicesGetter;
	AudioDataCapturerObservable observable_;
};
