#ifndef __CORE_HPP
#define __CORE_HPP

#include "stm32f10x.h"

namespace hw
{
namespace core
{
namespace config
{
// Timer used for delays
constexpr TIM_TypeDef* DelayTimer = TIM6;
// On-board LEDs port
constexpr GPIO_TypeDef* LedPort = GPIOC;
constexpr uint16_t Led3Pin = GPIO_Pin_9;
constexpr uint16_t Led4Pin = GPIO_Pin_8;
}

void setup();

void delay_ns(uint32_t nsec);
void delay_us(uint16_t usec);
void delay_ms(uint16_t msec);


inline void led3_on()
{
	::hw::core::config::LedPort->BSRR = ::hw::core::config::Led3Pin;
}

inline void led3_off()
{
	::hw::core::config::LedPort->BRR = ::hw::core::config::Led3Pin;
}

inline void led4_on()
{
	::hw::core::config::LedPort->BSRR = ::hw::core::config::Led4Pin;
}

inline void led4_off()
{
	::hw::core::config::LedPort->BRR = ::hw::core::config::Led4Pin;
}

}
}

#endif /* __CORE_HPP */
