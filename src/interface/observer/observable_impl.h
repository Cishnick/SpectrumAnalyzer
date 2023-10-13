#include "observable_controller_getter.h"

template <class ObservableObject>
Observable<ObservableObject>::ScopedUpdater::~ScopedUpdater()
{
	func_();
}

template <class ObservableObject>
ObservableObject *Observable<ObservableObject>::ScopedUpdater::operator->() const
{
	return &data_;
}

template <class ObservableObject>
ObservableObject &Observable<ObservableObject>::ScopedUpdater::operator*() const
{
	return data_;
}

template <class ObservableObject>
Observable<ObservableObject>::ScopedUpdater::ScopedUpdater(ObservableObject &data, std::function<void(void)> func)
	: data_(data)
	, func_(std::move(func))
{
}

template <class ObservableObject>
Observable<ObservableObject>::~Observable()
{
	for (const auto &[observer, _] : storage_)
		ObserverControllerGetter{*observer}.reset_unregister_function();
	storage_.clear();
}

template <class ObservableObject>
Observable<ObservableObject>::ScopedUpdater Observable<ObservableObject>::update()
{
	return ScopedUpdater(static_cast<ObservableObject &>(*this),
	                     [this]()
	                     {
							 for (const auto &[_, function] : storage_)
								 function(*static_cast<ObservableObject *>(this));
						 });
}

template <class ObservableObject>
void Observable<ObservableObject>::unregister_observer(ObserverController *observer)
{
	if (!observer)
		return;
	ObserverControllerGetter{*observer}.reset_unregister_function();
	storage_.erase(observer);
}

template <class ObservableObject>
void Observable<ObservableObject>::register_observer(ObserverController *observer, Observable::Function function)
{
	if (!observer)
		return;
	unregister_observer(observer);

	ObserverControllerGetter{*observer}.set_unregister_func([this](ObserverController *observer)
	                                                        { unregister_observer(observer); });
	storage_.emplace(observer, std::move(function));
}