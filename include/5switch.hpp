#ifndef __5SWITCH_HPP
#define __5SWITCH_HPP

#include "config.hpp"

class switch5
{
public:
	enum Position
	{
		NONE = 0,	// invalid state
		LL = 1,		// left-left
		L = 2,		// left
		M = 3,		// middle
		R = 4,		// right
		RR = 5		// right-right
	};

public:
	constexpr switch5(GPIO_TypeDef* port, uint8_t first_pin) :
			_port(port), _first_pin(first_pin), _state(0), _position(NONE)
	{
	}

	void setup();

	Position position() const
	{
		return _position;
	}

	// Should be called from SysTick interrupt handler.
	void scan();
private:
	switch5();

	GPIO_TypeDef* const _port;
	uint8_t const _first_pin; // First pin in the port. 3 consecutive pins are used.
	uint32_t _state; // Software debouncing state
	volatile Position _position; // Current position
};

#endif /* __5SWITCH_HPP */
