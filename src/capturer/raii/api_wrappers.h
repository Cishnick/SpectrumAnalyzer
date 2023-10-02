#pragma once
#include "src/app/stdafx.h"

#include <AudioClient.h>
#include <AudioPolicy.h>
#include <MMDeviceAPI.h>
#include <functiondiscoverykeys.h>
#include <mmdeviceapi.h>

#include <exception>
#include <stdexcept>

#include "raii_base.h"

class IMMDeviceEnumeratorPtr : public RaiiBase<IMMDeviceEnumerator>
{
public:
	explicit IMMDeviceEnumeratorPtr(DWORD dwClsContext)
		: RaiiBase<IMMDeviceEnumerator>(createInstance(dwClsContext),
	                                    [](IMMDeviceEnumerator *ptr)
	                                    {
											if (!ptr)
												return;
											ptr->Release();
										})
	{
	}

private:
	static IMMDeviceEnumerator *createInstance(DWORD dwClsContext)
	{
		IMMDeviceEnumerator *deviceEnumerator = nullptr;
		if (auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, dwClsContext, IID_PPV_ARGS(&deviceEnumerator));
		    FAILED(hr))
			throw std::exception("Error of deviceEnumerator initializing", hr);
		return deviceEnumerator;
	}
};

class IMMDeviceCollectionPtr : public RaiiBase<IMMDeviceCollection>
{
public:
	IMMDeviceCollectionPtr() = default;

	IMMDeviceCollectionPtr(IMMDeviceEnumeratorPtr deviceEnumerator, DWORD dwStateMask)
		: RaiiBase<IMMDeviceCollection>(createInstance(std::move(deviceEnumerator), dwStateMask),
	                                    [](IMMDeviceCollection *ptr)
	                                    {
											if (!ptr)
												return;
											ptr->Release();
										})
	{
	}

private:
	static IMMDeviceCollection *createInstance(IMMDeviceEnumeratorPtr deviceEnumerator, DWORD dwStateMask)
	{
		IMMDeviceCollection *deviceCollection;

		if (auto hr = deviceEnumerator->EnumAudioEndpoints(eCapture, dwStateMask, &deviceCollection); FAILED(hr))
			throw std::exception("Error of deviceCollection initializing", hr);

		return deviceCollection;
	}
};

class IMMDevicePtr : public RaiiBase<IMMDevice>
{
public:
	IMMDevicePtr() = default;

	IMMDevicePtr(const IMMDeviceCollectionPtr &deviceCollectionPtr, UINT deviceIndex)
		: RaiiBase<IMMDevice>(createInstance(deviceCollectionPtr, deviceIndex),
	                          [](IMMDevice *ptr)
	                          {
								  if (!ptr)
									  return;
								  ptr->Release();
							  })
	{
	}

private:
	static IMMDevice *createInstance(const IMMDeviceCollectionPtr &deviceCollectionPtr, UINT deviceIndex)
	{
		IMMDevice *device;
		if (auto hr = deviceCollectionPtr->Item(deviceIndex, &device); FAILED(hr))
			throw std::exception("Unable to get device", hr);
		return device;
	}
};

class IPropertyStorePtr : public RaiiBase<IPropertyStore>
{
public:
	IPropertyStorePtr() = default;

	IPropertyStorePtr(const IMMDevicePtr &device, DWORD stgmAccess)
		: RaiiBase<IPropertyStore>(createInstance(device, stgmAccess),
	                               [](IPropertyStore *ptr)
	                               {
									   if (!ptr)
										   return;
									   ptr->Release();
								   })
	{
	}

private:
	static IPropertyStore *createInstance(const IMMDevicePtr &device, DWORD stgmAccess)
	{
		IPropertyStore *propertyStore;
		if (auto hr = device->OpenPropertyStore(stgmAccess, &propertyStore); FAILED(hr))
			throw std::exception("Unable to get device", hr);
		return propertyStore;
	}
};

class PropVariantPtr : public RaiiBase<PROPVARIANT>
{
public:
	PropVariantPtr()
		: RaiiBase<PROPVARIANT>(createInstance(),
	                            [](PROPVARIANT *ptr)
	                            {
									PropVariantClear(ptr);
									delete ptr;
								})
	{
	}

private:
	static PROPVARIANT *createInstance()
	{
		auto instance = new PROPVARIANT;
		PropVariantInit(instance);
		return instance;
	}
};

class IAudioClientPtr : public RaiiBase<IAudioClient>
{
public:
	IAudioClientPtr() = default;

	IAudioClientPtr(const IMMDevicePtr &device, DWORD dwClsCtx)
		: RaiiBase<IAudioClient>(createInstance(device, dwClsCtx),
	                             [](IAudioClient *ptr)
	                             {
									 if (!ptr)
										 return;
									 ptr->Release();
								 })
	{
	}

private:
	static IAudioClient *createInstance(const IMMDevicePtr &device, DWORD dwClsCtx)
	{
		IAudioClient *client;
		if (auto hr = device->Activate(__uuidof(IAudioClient), dwClsCtx, nullptr, reinterpret_cast<void **>(&client));
		    FAILED(hr))
			throw std::exception("Unable to activate device", hr);

		return client;
	}
};

class WaveFormatExPtr : public RaiiBase<WAVEFORMATEX>
{
public:
	WaveFormatExPtr() = default;

	explicit WaveFormatExPtr(const IAudioClientPtr &audioClient)
		: RaiiBase<WAVEFORMATEX>(createInstance(audioClient),
	                             [](WAVEFORMATEX *ptr)
	                             {
									 if (!ptr)
										 return;
									 CoTaskMemFree(ptr);
								 })
	{
	}

private:
	static WAVEFORMATEX *createInstance(const IAudioClientPtr &audioClient)
	{
		WAVEFORMATEX *mixFormat;
		if (auto hr = audioClient->GetMixFormat(&mixFormat); FAILED(hr))
			throw std::exception("Unable to get mix format", hr);

		return mixFormat;
	}
};

class IAudioCaptureClientPtr : public RaiiBase<IAudioCaptureClient>
{
public:
	IAudioCaptureClientPtr() = default;

	explicit IAudioCaptureClientPtr(const IAudioClientPtr &client)
		: RaiiBase<IAudioCaptureClient>(createInstance(client),
	                                    [](IAudioCaptureClient *ptr)
	                                    {
											if (!ptr)
												return;
											ptr->Release();
										})
	{
	}

private:
	static IAudioCaptureClient *createInstance(const IAudioClientPtr &client)
	{
		IAudioCaptureClient *captureClient;
		if (auto hr = client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void **>(&captureClient)); FAILED(hr))
			throw std::exception("Unable to get capture client", hr);

		return captureClient;
	}
};
