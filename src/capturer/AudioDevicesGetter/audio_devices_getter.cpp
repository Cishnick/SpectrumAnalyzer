#include "src/app/stdafx.h"

#include "audio_devices_getter.h"

#include <AudioClient.h>
#include <AudioPolicy.h>
#include <MMDeviceAPI.h>
#include <functiondiscoverykeys.h>

namespace
{

std::string getDeviceName(const IMMDevicePtr &device, std::string_view defaultName)
{
	HRESULT hr;
	constexpr auto kDeviceNameBufferSize = 128;
	wchar_t wDeviceNameBuffer[kDeviceNameBufferSize];
	char deviceNameBuffer[kDeviceNameBufferSize];

	auto propertyStore = IPropertyStorePtr(device, STGM_READ);
	auto friendlyName = PropVariantPtr();

	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, friendlyName.get());
	if (FAILED(hr))
		throw std::exception("Unable to retrieve friendly name for device");

	if (friendlyName->vt != VT_LPWSTR)
		return std::string{defaultName};

	hr = StringCbPrintfW(wDeviceNameBuffer, sizeof(wDeviceNameBuffer), L"%s", friendlyName->pwszVal);
	if (FAILED(hr))
		throw std::exception("Unable to format friendly name for device");

	WideCharToMultiByte(CP_ACP, 0, wDeviceNameBuffer, -1, deviceNameBuffer, sizeof(deviceNameBuffer), 0, 0);
	return std::string{deviceNameBuffer};
}

size_t getDevicesSize(const IMMDeviceCollectionPtr &deviceCollection)
{
	UINT size;
	if (FAILED(deviceCollection->GetCount(&size)))
		throw std::exception("Unable to get device collection length");

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
		res.push_back(getDeviceName(IMMDevicePtr(deviceCollection_, i), "Unknown"));

	return res;
}

const IMMDeviceCollectionPtr &AudioDevicesGetter::getDeviceCollection() const
{
	return deviceCollection_;
}

size_t AudioDevicesGetter::getDevicesNumber() const
{
	return devicesNumber_;
}

void AudioDevicesGetter::update()
{
	deviceCollection_ = {IMMDeviceEnumeratorPtr(CLSCTX_SERVER), DEVICE_STATE_ACTIVE};
	devicesNumber_ = getDevicesSize(deviceCollection_);
}
