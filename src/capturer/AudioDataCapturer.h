#include "src/app/stdafx.h"

#include <thread>

#include "src/app/IAudioDataCapturer.h"
#include "src/app/RingBuffer.h"

// Объявление синонимов типов
using namespace std;

// Основной класс, реализующий интерфейс
class AudioDataCapturer : public IAudioDataCapturer
{
public:
	AudioDataCapturer();
	virtual ~AudioDataCapturer();

	VectorOfString GetDeviceList() override;

	void PickDevice(unsigned index) override;

	void add(IObserverAudio *obs) override;

	void remove(IObserverAudio *obs) override;

	void ChangeDevice(unsigned index) override;

	UINT32 GetSampleFrenq() override;

	void Initialize(size_t buffer_size) override;

	void Start() override;

	void Stop() override;

	void Release() override;

	bool isRun() override;

	void SetNSamples(int n) override;

	// Сообщает всем наблюдателям о наполнении буфера. Передает данные для обоих каналов
	// и частоту дискретизации
	void Handle(Reals const &r_chan, Reals const &l_chan, unsigned sample_frenq);

private:
	// Основная рабочая функция, которая выполняется в отдельном потоке
	void run();

	list<IObserverAudio *> _observers;
	IMMDevice *_device;
	IAudioClient *_AudioClient;
	IAudioCaptureClient *_CaptureClient;
	ERole _role;
	WAVEFORMATEX *_MixFormat;
	UINT32 _BufferSize;
	std::thread _thread;
	bool _isWorked;
	RingBuffer<double> _buffer_r;
	RingBuffer<double> _buffer_l;
	int _ind;

private:
	std::unique_ptr<class AudioDevicesGetter> audioDevicesGetter_;
};
