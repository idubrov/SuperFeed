#include "main.hpp"
#include "encoder.hpp"
#include "5switch.hpp"
#include "keypad.hpp"
#include "systick.hpp"
#include "util.hpp"
#include "lcd.hpp"
#include "driver.hpp"
#include "stepper.hpp"
#include "stm32f10x_gpio.h"

// Initialize subsystems

// First, enable clocks for utilized subsystems
class init {
private:
	static init g_instance;

	init() {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	}
};

// Initialize clocks, SysTick handler and util module
init init::g_instance;
systick systick::g_instance; // SysTick interrupt management
util util::g_instance;

//driver driver::g_instance;
//stepper g_stepper;

//		// FIXME: For CN0162 setup time is 10usec at least, need to check...
//
//
//		Port->BSRR = StepPin;
//		// Wait at least 75ns (for IMS483). Each NOP should be ~40ns (1/24000000 sec)
//		__NOP();
//		__NOP();
//
//		// for CN0162, wait time should be 300ns at least
//		//__NOP(); __NOP(); __NOP(); __NOP();
//		//__NOP(); __NOP(); __NOP(); __NOP();
//
//		Port->BRR = StepPin;

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main()
{
	// PC10-PC12 should be connected to the first three output of the switch.
	// 4th lead of the switch should be connected to GND.
	switch5 switch5(GPIOC, GPIO_PinSource10);

	// PA5 should be connected to encoder button, PA6 and PA7 to rotary encoder.
	encoder encoder(GPIOA, TIM3, GPIO_Pin_5, GPIO_Pin_6 | GPIO_Pin_7);

	// Columns start with pin PC0, rows start with PC4
	keypad keypad(GPIOC, GPIO_PinSource0, GPIO_PinSource4);

	lcd lcd(GPIOB, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7,
			GPIOB, GPIO_PinSource8);

	//	constexpr uint32_t ControlPortClock = RCC_APB2Periph_GPIOB;
	//	constexpr GPIO_TypeDef* ControlPort = GPIOB;
	//	constexpr uint16_t RSPin = GPIO_Pin_5;
	//	constexpr uint16_t RWPin = GPIO_Pin_6;
	//	constexpr uint16_t EPin = GPIO_Pin_7;
	//
	//	// Data port
	//	constexpr uint32_t DataPortClock = RCC_APB2Periph_GPIOB;
	//	constexpr GPIO_TypeDef* DataPort = GPIOB;
	//	constexpr uint16_t DataPins = GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
	//			                       GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	//	constexpr uint16_t DataShift = 8; // Data pins start with pin 8
	//	constexpr uint16_t BusyFlagPin = GPIO_Pin_15; // DB7
	//
	//	// Configuration
	//	constexpr bool UseBusyFlag = false;

//	bool dir = true;
//	while(1) {
//		for (int i = 0; i < 10000; i++) {
//			driver::step(Bit_SET);
//			util::led4_on();
//			util::delay_us(50);
//			driver::step(Bit_RESET);
//			util::led4_off();
//			util::delay_us(50);
//		}
//
//		util::delay_ms(1);
//		driver::direction(dir ? Bit_SET : Bit_RESET);
//		dir = !dir;
//	}
	encoder.limit(20);
	lcd.display(lcd.DisplayOn, lcd.CursorOff, lcd.BlinkOff);
	while (1) {
		lcd.position(0, 0);
//		if (encoder::pressed()) {
//			lcd.print("P ");
//			encoder::limit(10);
//		} else {
//			lcd.print("N ");
//		}
//
		lcd.print('0' + switch5.position());
		lcd.print(" ");
		lcd.print('0' + (GPIOC->IDR & GPIO_Pin_10 ? 1 : 0));
		lcd.print('0' + (GPIOC->IDR & GPIO_Pin_11 ? 1 : 0));
		lcd.print('0' + (GPIOC->IDR & GPIO_Pin_12 ? 1 : 0));
		
		lcd.position(0, 1);
		lcd.print(encoder.pressed() ? 'P' : 'N');
		int pos = encoder.position();
		lcd.print('0' + ((pos / 100) % 10));
		lcd.print('0' + ((pos / 10) % 10));
		lcd.print('0' + ((pos / 1) % 10));

		lcd.position(0, 2);
		int key = keypad.key();
		lcd.print('0' + (key / 10));
		lcd.print('0' + (key % 10));

		util::delay_ms(100);
	}
	return 0;
}

