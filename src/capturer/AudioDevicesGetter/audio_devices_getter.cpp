#include "src/app/stdafx.h"

#include "audio_devices_getter.h"

#include <AudioClient.h>
#include <AudioPolicy.h>
#include <MMDeviceAPI.h>
#include <functiondiscoverykeys.h>

namespace
{

string GetDeviceName(IMMDeviceCollection *DeviceCollection, UINT DeviceIndex)
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
	if (!returnValue)
	{
		ErrorMessage("Unable to allocate buffer for return", 0);
	}
	else
	{
		WideCharToMultiByte(CP_ACP, 0, returnValue, -1, const_cast<char *>(res.c_str()), 128, 0, 0);
	}

	return res;
}

IMMDeviceCollection *UpdateDeviceCollection()
{
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator = nullptr;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
		ErrorMessage("Unable to instantiate device enumerator", hr);

	IMMDeviceCollection *deviceCollection;
	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);

	if (FAILED(hr))
		ErrorMessage("Unable to retrieve device collection", hr);

	SafeRelease(&deviceEnumerator);

	return deviceCollection;
}

size_t getDevicesSize(IMMDeviceCollection *deviceCollection)
{
	UINT size;
	auto hr = deviceCollection->GetCount(&size);
	if (FAILED(hr))
		ErrorMessage("Unable to get device collection length", hr);
	return size;
}

} // namespace

AudioDevicesGetter::AudioDevicesGetter()
{
	update();
}

std::vector<std::string> AudioDevicesGetter::getDevicesList() const
{
	std::vector<std::string> res;

	for (UINT i = 0; i < devicesNumber_; i += 1)
		res.push_back(GetDeviceName(deviceCollection_, i));

	return res;
}

IMMDeviceCollection &AudioDevicesGetter::getDeviceCollection() const
{
	return *deviceCollection_;
}

size_t AudioDevicesGetter::getDevicesNumber() const
{
	return devicesNumber_;
}

void AudioDevicesGetter::update()
{
	if (deviceCollection_)
		deviceCollection_->Release();

	deviceCollection_ = UpdateDeviceCollection();
	devicesNumber_ = getDevicesSize(deviceCollection_);
}
