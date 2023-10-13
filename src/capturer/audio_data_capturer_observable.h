#pragma once

#include "src/interface/observer/observable.h"
#include <span>

struct AudioDataCapturerObservable : public Observable<AudioDataCapturerObservable>
{
	std::span<double> leftChannel;
	std::span<double> rightChannel;
	unsigned sampleFrequency;
};
