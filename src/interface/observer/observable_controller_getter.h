#pragma once
#include "observer_controller.h"

class ObserverControllerGetter
{
public:
	explicit ObserverControllerGetter(ObserverController &observer_controller)
		: observer_controller_(observer_controller)
	{
	}

	void set_unregister_func(ObserverController::UnregisterFunc func) { observer_controller_.unregister_ = std::move(func); }

	void reset_unregister_function() { observer_controller_.unregister_ = std::nullopt; }

private:
	ObserverController &observer_controller_;
};