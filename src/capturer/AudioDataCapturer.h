#include "src/app/stdafx.h"

#include <thread>
#include <atomic>

#include "raii/api_wrappers.h"
#include "src/app/IAudioDataCapturer.h"
#include "src/app/RingBuffer.h"

using namespace std;

// Основной класс, реализующий интерфейс
class AudioDataCapturer : public IAudioDataCapturer
{
public:
	AudioDataCapturer();
	virtual ~AudioDataCapturer();

	std::vector<std::string> getDeviceList() override;

	void add(IObserverAudio *obs) override;

	void remove(IObserverAudio *obs) override;

	void changeDevice(unsigned index) override;

	UINT32 GetSampleFrenq() override;


	void start() override;

	void stop() override;

	bool isRunning() override;

	void setFifoSize(int n) override;

	// Сообщает всем наблюдателям о наполнении буфера. Передает данные для обоих каналов
	// и частоту дискретизации
	void Handle(Reals const &r_chan, Reals const &l_chan, unsigned sample_frenq);

private:
	void initialize(size_t fifo_size);

	// For call from destructor instead virtual function 'stop'
	void stop_impl();

	// Основная рабочая функция, которая выполняется в отдельном потоке
	void run();

private:
	std::list<IObserverAudio *> _observers;
	IAudioClientPtr audioClient;
	IAudioCaptureClientPtr captureClient;
	WaveFormatExPtr mixFormat;
	UINT32 bufferSize;
	std::thread thread;
	std::atomic_bool running;
	RingBuffer<double> _buffer_r;
	RingBuffer<double> _buffer_l;
	unsigned currentDeviceIndex;

private:
	std::unique_ptr<class AudioDevicesGetter> audioDevicesGetter;
};
