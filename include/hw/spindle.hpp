#ifndef __SPINDLE_HPP
#define __SPINDLE_HPP

#include "stm32f10x.h"
#include <atomic>

namespace hw
{

class spindle
{
public:
public:
	constexpr spindle(TIM_TypeDef* index_timer, GPIO_TypeDef* index_port, uint_fast16_t index_pin) :
			_index_timer(index_timer), _index_port(index_port), _index_pin(index_pin),
			_delay(0), _overflowed(false)
	{
	}
	spindle(spindle const&) = delete;
	void initialize();

	bool raw_index() const
	{
		return !(_index_port->IDR & _index_pin);
	}

	uint_fast16_t raw_delay() const
	{
		return _delay.load(std::memory_order_relaxed);
	}

	void overflow_handler();
	void index_pulse_hanlder();
private:
	TIM_TypeDef* const _index_timer;
	GPIO_TypeDef* const _index_port;
	uint_fast16_t const _index_pin;

	std::atomic_uint_fast16_t _delay; // Updated from the IRQ handler
	bool _overflowed = false; // Only used in IRQ handler
};
}

template<typename S>
inline S const& operator<<(S const& l, hw::spindle const& spindle)
{
	l << "I: " << spindle.raw_index();
	return l;
}

#endif /* __SPINDLE_HPP */
