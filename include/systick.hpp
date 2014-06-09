#ifndef __SYSTICK_HPP
#define __SYSTICK_HPP

#include "config.hpp"
#include "FastDelegate.h"
#include "cortexm/ExceptionHandlers.h"
#include <stdlib.h>

class systick
{
	friend void ::SysTick_Handler();
	static constexpr int MaxHandlers = 4;
public:
	static systick instance() {
		return g_instance;
	}

	template <typename X>
	static void bind(X* pthis, void (X::*handler)()) {
		// FIXME-isd: disable interrupts!!!
		for (int i = 0; i < MaxHandlers; ++i) {
			if (_handlers[i].empty()) {
				_handlers[i].bind(pthis, handler);
				return;
			}
		}
		abort(); // No more empty slots!
	}

private:
	static void tick() {
		for (int i = 0; i < MaxHandlers; i++) {
			if (!_handlers[i].empty()) {
				_handlers[i]();
			}
		}
	}
private:
	typedef fastdelegate::FastDelegate0<> TickHandler;
	static TickHandler _handlers[MaxHandlers];

	static systick g_instance;
	systick();
};

#endif /* __SYSTICK_HPP */
