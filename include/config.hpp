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
// C KKKKKKKK##SSS+++
// D +++
//
// XX STLINK connection
// ## on-board LEDs
// L LCD display
// E rotary encoder
// D stepper motor driver (not yet configured!)
// K keypad

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

// PC10-PC12 should be connected to first three output of the switch.
// 4th lead of the switch should be connected to GND.
namespace sw5
{
//constexpr uint32_t PortClock = RCC_APB2Periph_GPIOC;
//constexpr GPIO_TypeDef* Port = GPIOC;
//constexpr uint32_t Pins = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
//constexpr uint32_t PinShift = 10; // 3 pins starting from pin 10
}

// PA5 should be connected to encoder button, PA6 and PA7 to rotary encoder.
namespace encoder {
//constexpr uint32_t PortClock = RCC_APB2Periph_GPIOA;
//constexpr uint32_t TimerClock = RCC_APB1Periph_TIM3;
//constexpr TIM_TypeDef* Timer = TIM3;
//
//constexpr GPIO_TypeDef* Port = GPIOA;
//constexpr uint16_t ButtonPin = GPIO_Pin_5;
//constexpr uint16_t EncoderPins = GPIO_Pin_6 | GPIO_Pin_7;
}

// PC0-PC7 should be connected to keypad pins.
namespace keypad {
//constexpr uint32_t PortClock = RCC_APB2Periph_GPIOC;
//constexpr GPIO_TypeDef* Port = GPIOC;
//constexpr uint16_t ColumnsPins = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
//constexpr uint16_t ColumnsShift = 0; // Rows start with pin 0
//constexpr uint16_t RowsPins = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//constexpr uint16_t RowsShift = 4; // Columns start with pin 4
}

/// PB8-PB15 should be connected to D0-D7
/// PB5 to RS, PB6 to R/W, PB7 to E
namespace lcd {
// Control port
constexpr uint32_t ControlPortClock = RCC_APB2Periph_GPIOB;
constexpr GPIO_TypeDef* ControlPort = GPIOB;
constexpr uint16_t RSPin = GPIO_Pin_5;
constexpr uint16_t RWPin = GPIO_Pin_6;
constexpr uint16_t EPin = GPIO_Pin_7;

// Data port
constexpr uint32_t DataPortClock = RCC_APB2Periph_GPIOB;
constexpr GPIO_TypeDef* DataPort = GPIOB;
constexpr uint16_t DataPins = GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
		                       GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
constexpr uint16_t DataShift = 8; // Data pins start with pin 8
constexpr uint16_t BusyFlagPin = GPIO_Pin_15; // DB7

// Configuration
constexpr bool UseBusyFlag = false;
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

// CN0162
//constexpr uint32_t StepLen = 300;
//constexpr uint32_t StepSpace = 300;
//constexpr uint32_t DirectionSetup = 10000;
//constexpr uint32_t DirectionHold = 0;
}
}

#endif /* __UTIL_HPP */
