#pragma once

#include <functional>
#include <map>

class ObserverController;

template <class ObservableObject>
class Observable
{
protected:
	virtual ~Observable();

public:
	using Function = std::function<void(const ObservableObject &)>;

	void register_observer(ObserverController *observer, Function function);

	void unregister_observer(ObserverController *observer);

	class ScopedUpdater
	{
	public:
		using UpdateFunc = std::function<void(void)>;
		ScopedUpdater(const ScopedUpdater &) = delete;

		explicit ScopedUpdater(ObservableObject &data, UpdateFunc func);

		ObservableObject &operator*() const;

		ObservableObject *operator->() const;

		~ScopedUpdater();

	private:
		ObservableObject &data_;
		UpdateFunc func_;
	};

	ScopedUpdater update();

private:
	std::map<ObserverController *, Function> storage_{};
};

#include "observable_impl.h"
