#ifndef __SPINDLE_HPP
#define __SPINDLE_HPP

#include "stm32f10x.h"

namespace hw
{

class spindle
{
public:
public:
	constexpr spindle(TIM_TypeDef* index_timer, GPIO_TypeDef* index_port, uint16_t index_pin) :
			_index_timer(index_timer), _index_port(index_port), _index_pin(index_pin),
			_captured(0), _overflowed(false)
	{
	}
	spindle(spindle const&) = delete;

	bool raw_index() const
	{
		return !(_index_port->IDR & _index_pin);
	}

	uint16_t raw_speed() const
	{
		return _captured;
	}
	void initialize();

	void overflow_handler();
	void index_pulse_hanlder();
private:
	TIM_TypeDef* const _index_timer;
	GPIO_TypeDef* const _index_port;
	uint16_t const _index_pin;

	uint16_t volatile _captured;
	bool _overflowed;

};
}

template<typename S>
inline S const& operator<<(S const& l, hw::spindle const& spindle)
{
	l << "I: " << spindle.raw_index();
	return l;
}

#endif /* __SPINDLE_HPP */
