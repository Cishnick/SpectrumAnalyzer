#pragma once
#include <memory>

template <typename T>
using DefaultDeleter = std::function<void(T*)>;

namespace PropagateConstPtrDetails
{
template <class T>
struct PropagateConstPtrCreator;

template <class T>
struct PropagateConstPtrCreator<std::shared_ptr<T>>
{
	template <class... ARGS>
	std::shared_ptr<T> operator()(ARGS &&...args)
	{
		return std::make_shared<T>(std::forward<ARGS>(args)...);
	}
};

template <class T>
struct PropagateConstPtrCreator<std::unique_ptr<T>>
{
	template <class... ARGS>
	std::unique_ptr<T> operator()(ARGS &&...args)
	{
		return std::make_unique<T>(std::forward<ARGS>(args)...);
	}
};

} // namespace PropagateConstPtrDetails

template <class T, template <class...> class P = std::unique_ptr, class Deleter = DefaultDeleter<T>>
class PropagateConstPtr
{
public:
	template <class... ARGS>
	explicit PropagateConstPtr(ARGS &&...args)
		: m_p(PropagateConstPtrDetails::PropagateConstPtrCreator<P<T, Deleter>>()(std::forward<ARGS>(args)...))
	{
	}

	explicit PropagateConstPtr(P<T> &&p)
		: m_p(std::move(p))
	{
	}

	~PropagateConstPtr() = default;

	const T *get() const noexcept { return m_p.get(); }

	T *get() noexcept { return m_p.get(); }

	const T *operator->() const noexcept { return get(); }

	T *operator->() noexcept { return get(); }

	const T &operator*() const noexcept { return *get(); }

	T &operator*() noexcept { return *get(); }

	explicit operator const T *() const noexcept { return get(); }

	explicit operator T *() noexcept { return get(); }

	void swap(PropagateConstPtr &rhs) noexcept { rhs.m_p.swap(m_p); }

	void reset() { m_p.reset(); }

	void reset(T *p) noexcept { m_p.reset(); }

	void reset(P<T> *p) noexcept { m_p.reset(); }

	explicit operator bool() const noexcept { return static_cast<bool>(get()); }

	bool operator!() const noexcept { return !get(); }

private:
	P<T> m_p;
};
