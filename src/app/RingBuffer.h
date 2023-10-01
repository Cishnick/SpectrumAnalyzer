#pragma once
#include "stdafx.h"

template <typename T>
class RingBuffer
{
public:
	explicit RingBuffer(size_t size)
		: _size(size)
		, _cursor(0)
		, _buf1(size)
		, _buf2(size)
		, _whichBuffer(false)
	{
	}

	std::vector<T> *push_back(T const &element)
	{
		std::vector<T> *pBuffer = (_whichBuffer) ? &_buf2 : &_buf1;
		pBuffer->at(_cursor++) = element;
		if (_cursor == _size)
		{
			_cursor = 0;
			_whichBuffer = !_whichBuffer;
			return pBuffer;
		}
		return nullptr;
	}

	size_t getSize() const { return _size; }

	void Reset()
	{
		_buf1.clear();
		_buf2.clear();
		_cursor = 0;
		_whichBuffer = false;
	}

	void Resize(size_t new_n)
	{
		Reset();
		_buf1.resize(new_n);
		_buf2.resize(new_n);
	}

private:
	std::vector<T> _buf1;
	std::vector<T> _buf2;
	size_t _size;
	size_t _cursor;
	bool _whichBuffer;
};
