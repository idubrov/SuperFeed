#ifndef __RESOURCES_HPP
#define __RESOURCES_HPP

#include "stm32f10x.h"

//
// Global resources definitions. All other modules use these constants
// to access peripherals. These constants are internal and should only
// be used by corresponding module.
//
// Resources table
//   0123456789ABCDEF
// A +++++EEE++DDDXXD
// B +++XXLLLLLLLLLLL
// C KKKKKKKK##SSSXXX
// D +++
//
// XX STLINK connection, low current ports, etc
// ## on-board LEDs
// L LCD display
// E rotary encoder
// D stepper motor driver
// K keypad
//
// Timers:
// TIM1 -- slave timer for stepper PWM (signal generation)?
// TIM2 -- quadrature encoder for spindle
// TIM3 -- rotary encoder
// TIM4 -- master timer for stepper PWM?
// TIM6 -- delay timer
// TI15 -- indexing for spindle
//
// TODO:
// PA0, PA1 -- TIM2, quadrature encoder for spindle
// PA2 -- TIM15, indexing for spindle
// PA8 -- TIM1 ch1, re-route to driver step,  from PA15

//
namespace cfg
{
// Utility module timer and LEDs
namespace util
{
// Timer used for delays
constexpr uint32_t DelayTimerClock = RCC_APB1Periph_TIM6;
constexpr TIM_TypeDef* DelayTimer = TIM6;

constexpr uint32_t LedPortClock = RCC_APB2Periph_GPIOC;
constexpr GPIO_TypeDef* LedPort = GPIOC;
constexpr uint16_t Led3Pin = GPIO_Pin_9;
constexpr uint16_t Led4Pin = GPIO_Pin_8;
}

// Driver should be connected as following:
// PA10 STEP
// PA11 DIR
// PA12 EN
// PA15 RESET
namespace driver {
// Control port
constexpr uint32_t PortClock = RCC_APB2Periph_GPIOA;
constexpr GPIO_TypeDef* Port = GPIOA;
constexpr uint16_t StepPin = GPIO_Pin_10;
constexpr uint16_t DirPin = GPIO_Pin_11;
constexpr uint16_t EnablePin = GPIO_Pin_12;
constexpr uint16_t ResetPin = GPIO_Pin_15;
}

// Stepper motor configuration
namespace stepper {
constexpr uint32_t Microsteps = 16;
constexpr uint32_t LeadscrewTPI = 16;
constexpr uint32_t StepsPerRev = 200;


// All delays are in nanoseconds
// IM483
constexpr uint32_t StepLen = 1000;
constexpr uint32_t StepSpace = 1000;
constexpr uint32_t DirectionSetup = 1000;
constexpr uint32_t DirectionHold = 1000;

// Note that you also would need to change IRQs in stepper.cpp
constexpr TIM_TypeDef* OutputTimer = TIM1;
constexpr TIM_TypeDef* StepperTimer = TIM4;
constexpr DMA_Channel_TypeDef* DMAChannel = DMA1_Channel7; // Update of TIM4

// CN0162
//constexpr uint32_t StepLen = 300;
//constexpr uint32_t StepSpace = 300;
//constexpr uint32_t DirectionSetup = 10000;
//constexpr uint32_t DirectionHold = 0;
}
}

#endif /* __UTIL_HPP */
