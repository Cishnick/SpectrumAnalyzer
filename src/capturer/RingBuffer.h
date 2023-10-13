#pragma once
#include "src/app/stdafx.h"
#include <optional>

template <typename T>
class RingBuffer
{
public:
	explicit RingBuffer(size_t size = 0)
		: size(size)
		, cursor(0)
		, buf1(size)
		, buf2(size)
		, whichBuffer(false)
	{
	}

	std::optional<std::vector<T>> pushBack(T const &element)
	{
		auto &currentBuffer = (whichBuffer) ? buf2 : buf1;
		currentBuffer.at(cursor++) = element;
		if (cursor == size)
		{
			cursor = 0;
			whichBuffer = !whichBuffer;
			return currentBuffer;
		}
		return std::nullopt;
	}

	[[nodiscard]] size_t getSize() const { return size; }

	void reset()
	{
		buf1.clear();
		buf2.clear();
		cursor = 0;
		whichBuffer = false;
	}

private:
	std::vector<T> buf1;
	std::vector<T> buf2;
	size_t size;
	size_t cursor;
	bool whichBuffer;
};
