#ifndef __BUTTONS_HPP
#define __BUTTONS_HPP

#include "stm32f10x.h"

namespace hw
{

class buttons
{
public:
public:
	constexpr buttons(GPIO_TypeDef* port, uint8_t first_pin) :
			_port(port), _first_pin(first_pin)
	{
	}
	buttons(buttons const&) = delete;

	uint8_t raw_buttons() const
	{
		return (_port->IDR >> _first_pin) & 7;
	}

	void initialize();
private:
	GPIO_TypeDef* const _port;
	uint8_t const _first_pin; // First pin in the port. 3 consecutive pins are used.
};
}

template<typename S>
inline S const& operator<<(S const& l, hw::buttons const& buttons)
{
	l << "B: " << buttons.raw_buttons();
	return l;
}

#endif /* __BUTTONS_HPP */
