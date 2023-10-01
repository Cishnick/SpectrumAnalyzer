#pragma once

#include <list>
#include <string>

class AudioDevicesGetter
{
public:
	AudioDevicesGetter();

	std::vector<std::string> getDevicesList() const;
	class IMMDeviceCollection &getDeviceCollection() const;
	size_t getDevicesNumber() const;

	void update();
private:


private:
	IMMDeviceCollection* deviceCollection_ = nullptr;
	size_t devicesNumber_ = 0;
};
