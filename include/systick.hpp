#ifndef __SYSTICK_HPP
#define __SYSTICK_HPP

#include "config.hpp"
#include "delegate.hpp"

#include <stdlib.h>

extern "C" {
void SysTick_Handler();
}

class systick
{
	friend void ::SysTick_Handler();
	static constexpr int MaxHandlers = 4;

	using TickHandler = delegate::Delegate<void ()>;
public:
	static void setup();

	static void bind(const TickHandler& delegate) {
		// Disable SysTick interrupts while we are adding a new handler
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
		find_empty_slot() = delegate;
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	}
private:
	static TickHandler& find_empty_slot() {
		for (auto& handler : _handlers) {
			if (handler.is_empty()) {
				return handler;
			}
		}
		abort(); // No more empty slots!
	}

private:
	static void tick() {
		for (auto& handler : _handlers) {
			if (!handler.is_empty()) {
				handler();
			}
		}
	}
private:
	static TickHandler _handlers[MaxHandlers];
};

#endif /* __SYSTICK_HPP */
