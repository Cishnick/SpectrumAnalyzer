#pragma once
#include <functional>
#include <memory>

template <class T>
class RaiiBase
{
public:
	using Deleter = std::function<void(T *)>;

	RaiiBase(const RaiiBase &) = delete;
	RaiiBase &operator=(const RaiiBase &) = delete;

	RaiiBase(RaiiBase &&other) noexcept
	{
		data_ = std::move(other.data_);
		other.data_ = {};
	}

	RaiiBase &operator=(RaiiBase &&other) noexcept
	{
		if (this == &other)
			return *this;

		data_ = std::move(other.data_);
		other.data_ = {};
		return *this;
	}

	RaiiBase(T *ptr, Deleter deinitializer)
		: data_(ptr, std::move(deinitializer))
	{
	}

	RaiiBase() = default;

	T *get() const { return data_.get(); }

	void reset(T *ptr) { data_.reset(ptr); }

	void reset(RaiiBase<T> &&obj)
	{
		data_.reset();
		data_.swap(obj.data_);
	}

	T *release() { return data_.release(); }

	typename std::add_lvalue_reference<T>::type operator*() const { return data_.operator*(); }

	T *operator->() const { return data_.operator->(); }

private:
	std::unique_ptr<T, Deleter> data_{};
};
