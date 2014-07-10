#ifndef __CORE_HPP
#define __CORE_HPP

#include "config.hpp"
#include "stm32f10x.h"

namespace hw
{
namespace core
{
void setup();

void delay_us(uint16_t usec);
void delay_ms(uint16_t msec);

inline void led3_on()
{
	::cfg::util::LedPort->BSRR = ::cfg::util::Led3Pin;
}

inline void led3_off()
{
	::cfg::util::LedPort->BRR = ::cfg::util::Led3Pin;
}

inline void led4_on()
{
	::cfg::util::LedPort->BSRR = ::cfg::util::Led4Pin;
}

inline void led4_off()
{
	::cfg::util::LedPort->BRR = ::cfg::util::Led4Pin;
}

}
}

#endif /* __CORE_HPP */
