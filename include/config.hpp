#ifndef __CONFIG_HPP
#define __CONFIG_HPP

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
// TIM1 -- step signal generator
// TIM2 -- quadrature encoder for spindle (not used yet)
// TIM3 -- rotary encoder
// TIM6 -- delay timer
// TIM15 -- indexing for spindle
//
// TODO:
// PA0, PA1 -- TIM2, quadrature encoder for spindle
// PA2 -- TIM15, indexing for spindle

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

namespace spindle
{
constexpr uint32_t Clock = 24000000; // Clock frequency
constexpr uint32_t Prescaler = 23999;
constexpr uint32_t SpindleTicksPerSec = Clock / (Prescaler + 1);
}

// Stepper motor configuration
namespace stepper
{
constexpr uint32_t Clock = 24000000; // Clock frequency
constexpr uint32_t Prescaler = 23;
constexpr uint32_t TicksPerSec = Clock / (Prescaler + 1);

// Hardware configuration
constexpr uint32_t Microsteps = 1;
constexpr uint32_t LeadscrewTPI = 16;
constexpr uint32_t GearingNom = 1; // Gearing nominator
constexpr uint32_t GearingDenom = 1; // Gearing denominator
constexpr uint32_t FullSteps = 200; // Steps per revolution
constexpr uint32_t Acceleration = 10000; // Steps per second per second

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
static constexpr uint32_t StepsPerInch = FullSteps * Microsteps * LeadscrewTPI;

// Limits
namespace limits
{
// Maximum amount of pulses per second (driver limitation)
constexpr uint32_t MaxPulseDR = 1000000000 / (StepLen + StepSpace);

// Our guestimation how many cycles one update could take
constexpr uint32_t CyclesPerUpdate = 100;
// Maximum amount of pulses per second (timer limitation)
constexpr uint32_t MaxPulseTIM = (Clock + CyclesPerUpdate - 1) / CyclesPerUpdate;

// Maximum full steps per second
constexpr uint32_t MaxPulse = MaxPulseTIM < MaxPulseDR ? MaxPulseTIM : MaxPulseDR;
constexpr uint32_t MaxFullPS = MaxPulse / Microsteps; // floor rounding

// Minimum amount of pulses per second (assuming 1usec period timer), limited by timer
constexpr uint32_t MaxDelay = 60000; // Maximum timer delay is around 2^16 ticks.
constexpr uint32_t MinPulsePS = TicksPerSec / 60000;
constexpr uint32_t MinFullPulsePS = (MinPulsePS + Microsteps - 1) / Microsteps; // ceil rounding
}
}
}

#endif /* __CONFIG_HPP */
