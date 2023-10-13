#pragma once
#include <functional>
#include <optional>

class ObserverControllerGetter;

class ObserverController
{
private:
	using UnregisterFunc = std::function<void(ObserverController *)>;
	std::optional<UnregisterFunc> unregister_{std::nullopt};
	friend class ObserverControllerGetter;

public:
	virtual ~ObserverController()
	{
		if (unregister_)
			(*unregister_)(this);
	}
};