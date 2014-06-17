#ifndef __UTIL_HPP
#define __UTIL_HPP

#include "config.hpp"

class util
{
public:
	static void setup();

	static void delay_us(uint16_t usec);
	static void delay_ms(uint16_t msec);

	static inline void led3_on()
	{
		::cfg::util::LedPort->BSRR = ::cfg::util::Led3Pin;
	}

	static inline void led3_off()
	{
		::cfg::util::LedPort->BRR = ::cfg::util::Led3Pin;
	}

	static inline void led4_on()
	{
		::cfg::util::LedPort->BSRR = ::cfg::util::Led4Pin;
	}

	static inline void led4_off()
	{
		::cfg::util::LedPort->BRR = ::cfg::util::Led4Pin;
	}
};

#endif /* __UTIL_HPP */
