#pragma once

#include <list>
#include <string>

#include "src/capturer/raii/api_wrappers.h"

class AudioDevicesGetter
{
public:
	AudioDevicesGetter();

	std::vector<std::string> getDevicesList() const;
	const IMMDeviceCollectionPtr &getDeviceCollection() const;
	size_t getDevicesNumber() const;

	void update();

private:
	IMMDeviceCollectionPtr deviceCollection_;
	size_t devicesNumber_ = 0;
};
