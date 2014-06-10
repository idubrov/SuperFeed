#ifndef __SYSTICK_HPP
#define __SYSTICK_HPP

#include "config.hpp"
#include "delegate.hpp"

#include "cortexm/ExceptionHandlers.h"
#include <stdlib.h>


class systick
{
	friend void ::SysTick_Handler();
	static constexpr int MaxHandlers = 4;

	using TickHandler = delegate::Delegate<void ()>;
public:
	static systick instance() {
		return g_instance;
	}

	static void bind(const TickHandler& delegate) {
		// Disable SysTick interrupts while we are adding a new handler
		NVIC_DisableIRQ(SysTick_IRQn);
		find_empty_slot() = delegate;
		NVIC_EnableIRQ(SysTick_IRQn);
	}
private:
	static TickHandler& find_empty_slot() {
		for (int i = 0; i < MaxHandlers; ++i) {
			if (_handlers[i].empty()) {
				return _handlers[i];
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
	static TickHandler _handlers[MaxHandlers];

	static systick g_instance;
	systick();
};

#endif /* __SYSTICK_HPP */
