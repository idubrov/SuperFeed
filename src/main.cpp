#include "main.hpp"
#include "encoder.hpp"
#include "5switch.hpp"
#include "keypad.hpp"
#include "systick.hpp"
#include "util.hpp"
#include "lcd.hpp"
#include "driver.hpp"
#include "stepper.hpp"
#include "simstream.hpp"
#include "stm32f10x_gpio.h"


// Initialize subsystems

// First, enable clocks for utilized subsystems
class init {
private:
	static init g_instance;

	init() {
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}
};

// Initialize clocks, SysTick handler and util module
init init::g_instance;
systick systick::g_instance; // SysTick interrupt management
util util::g_instance;

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

lcd* g_lcd;
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

	// LCD display, RS pin should be connected to PB5, R/W to PB6 and E to PB7
	// D0-D7 should be connected to PB8-PB15
	lcd lcd(GPIOB, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7,
			GPIOB, GPIO_PinSource8);
	//lcds lcd(lcd);
	g_lcd = &lcd;

	stepper stepper(TIM4, TIM15);

	switch5.initialize();
	encoder.initialize();
	keypad.initialize();
	lcd.initialize();
	lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
	//stepper.initialize();

	encoder.limit(20);

	lcd.clear();
	while (1) {
		lcd.write('a');
		//lcd << position(0, 0) << "Switch: " << switch5.position();
		//lcd << position(0, 1) << (encoder.pressed() ? 'P' : 'N') << ' ' << encoder.position();
		//lcd << position(0, 2) << (char) keypad.key();
		util::delay_ms(100);
	}
	return 0;
}

