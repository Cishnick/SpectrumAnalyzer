#include "src/app/stdafx.h"

#include "AudioDataCapturer.h"

IAudioDataCapturer *FactoryCapturer::make()
{
	return new AudioDataCapturer();
}

// -------------------------------------------Конструктор------------------------------------
AudioDataCapturer::AudioDataCapturer()
	: _deviceCollection(nullptr)
	, _device(nullptr)
	, _role(ERole::eMultimedia)
	, _buffer_r(0)
	, _buffer_l(0)
	, _AudioClient(NULL)
	, _CaptureClient(NULL)
	, _MixFormat(NULL)
	, _BufferSize(0)
	, _thread()
	, _isWorked(false)
	, _NofDev(0)
{
}

// -------------------------------------------------GetDeviceList----------------------------

VectorOfString AudioDataCapturer::GetDeviceList()
{
	VectorOfString res;
	UpdateDeviceCollection();
	for (UINT i = 0; i < _NofDev; i += 1)
		res.push_back(GetDeviceName(_deviceCollection, i));

	return res;
}

// ------------------------------------------------PickDevice--------------------------------

void AudioDataCapturer::PickDevice(unsigned index)
{
	HRESULT hr;
	if (index >= _NofDev)
	{
		ErrorMessage("Wrong device index", index);
		return;
	}
	hr = _deviceCollection->Item(index, &_device);
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
	SafeRelease(&_deviceCollection);
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

	UpdateDeviceCollection();
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

	UpdateDeviceCollection();
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

// -----------------------------------------------GetDevice----------------------------------

string AudioDataCapturer::GetDeviceName(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
{
	IMMDevice *device;
	HRESULT hr;
	string res(128, ' ');
	IPropertyStore *propertyStore;
	PROPVARIANT friendlyName;
	wchar_t deviceName[128];
	wchar_t *returnValue;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
		ErrorMessage("Unable to get device", hr);

	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	SafeRelease(&device);
	if (FAILED(hr))
		ErrorMessage("Unable to open device property store", hr);

	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	SafeRelease(&propertyStore);

	if (FAILED(hr))
	{
		ErrorMessage("Unable to retrieve friendly name for device", hr);
	}

	hr = StringCbPrintfW(deviceName,
	                     sizeof(deviceName),
	                     L"%s",
	                     friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal);
	if (FAILED(hr))
	{
		ErrorMessage("Unable to format friendly name for device", hr);
	}

	PropVariantClear(&friendlyName);

	returnValue = _wcsdup(deviceName);
	if (returnValue == NULL)
	{
		ErrorMessage("Unable to allocate buffer for return", 0);
	}
	else
	{
		WideCharToMultiByte(CP_ACP, 0, returnValue, -1, const_cast<char *>(res.c_str()), 128, 0, 0);
	}

	return res;
}

// ------------------------------------------------------UpdateDeviceCollection---------------

void AudioDataCapturer::UpdateDeviceCollection()
{
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator = nullptr;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
		ErrorMessage("Unable to instantiate device enumerator", hr);

	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &_deviceCollection);
	if (FAILED(hr))
		ErrorMessage("Unable to retrieve device collection", hr);

	SafeRelease(&deviceEnumerator);

	hr = _deviceCollection->GetCount(reinterpret_cast<UINT *>(&_NofDev));
	if (FAILED(hr))
		ErrorMessage("Unable to get device collection length", hr);
}

// ---------------------------------------------------run------------------------------------

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
