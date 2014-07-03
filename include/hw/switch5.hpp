#ifndef __SWITCH5_HPP
#define __SWITCH5_HPP

#include "stm32f10x.h"

namespace hw
{

class switch5
{
public:
	enum Position
	{
		None = 0,	// invalid state
		LL = 1,		// left-left
		L = 2,		// left
		M = 3,		// middle
		R = 4,		// right
		RR = 5		// right-right
	};

public:
	constexpr switch5(GPIO_TypeDef* port, uint8_t first_pin) :
			_port(port), _first_pin(first_pin)
	{
	}
	switch5(switch5 const&) = delete;

	Position raw_position() const;

	void initialize();
private:
	GPIO_TypeDef* const _port;
	uint8_t const _first_pin; // First pin in the port. 3 consecutive pins are used.
};
}

template<typename S>
inline S const& operator<<(S const& l, hw::switch5 const& switch5)
{
	l << "K: " << switch5.raw_position();
	return l;
}

#endif /* __SWITCH5_HPP */
