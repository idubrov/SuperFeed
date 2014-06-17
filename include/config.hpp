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

// Stepper motor configuration
namespace stepper
{
constexpr uint32_t MicroSteps = 16;
constexpr uint32_t LeadscrewTPI = 16;
constexpr uint32_t StepsPerRev = 200;

// All delays are in nanoseconds
// IM483
constexpr uint32_t StepLen = 1000;
constexpr uint32_t StepSpace = 1000;
constexpr uint32_t DirectionSetup = 1000;
constexpr uint32_t DirectionHold = 1000;

// CN0162
//constexpr uint32_t StepLen = 300;
//constexpr uint32_t StepSpace = 300;
//constexpr uint32_t DirectionSetup = 10000;
//constexpr uint32_t DirectionHold = 0;

// Derived parameters
static constexpr uint32_t MicrostepsPerInch =
		StepsPerRev * MicroSteps * LeadscrewTPI;

// Limits
namespace limits
{
constexpr uint32_t F = 24000000; // Timer frequency
constexpr uint32_t Prescaler = 23;
constexpr uint32_t TicksPerSec = F / (Prescaler + 1);

// Maximum amount of pulses per second (driver limitation)
constexpr uint32_t MaxPulseDR = 1000000000 / (StepLen + StepSpace);
// Maximum amount of pulses per second (timer limitation)
constexpr uint32_t MaxPulseTIM = TicksPerSec / 2; // Period = 2 ticks

// Maximum full steps per second
constexpr uint32_t MaxPulse = MaxPulseTIM < MaxPulseDR ? MaxPulseTIM : MaxPulseDR;
constexpr uint32_t MaxFullPS = MaxPulse / MicroSteps; // floor rounding

// Minimum amount of pulses per second (assuming 1usec period timer), limited by timer
constexpr uint32_t MaxDelay = 60000; // Maximum timer delay is around 2^16 ticks.
constexpr uint32_t MinPulsePS = TicksPerSec / 60000;
constexpr uint32_t MinFullPulsePS = (MinPulsePS + MicroSteps - 1) / MicroSteps; // ceil rounding
}
}
}

#endif /* __UTIL_HPP */
