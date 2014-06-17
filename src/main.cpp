#include "main.hpp"
#include "encoder.hpp"
#include "5switch.hpp"
#include "keypad.hpp"
#include "systick.hpp"
#include "util.hpp"
#include "HD44780.hpp"
#include "driver.hpp"
#include "stepper.hpp"
#include "simstream.hpp"
#include "stm32f10x_gpio.h"

extern "C" {
	void SysTick_Handler();
	void TIM4_IRQHandler();
}

// First, enable clocks for utilized subsystems
class application
{
	friend void ::TIM4_IRQHandler();
public:
	application() :
			// PC10-PC12 should be connected to the first three output of the switch.
			// 4th lead of the switch should be connected to GND.
			_switch5(GPIOC, GPIO_PinSource10),
			// PA5 should be connected to encoder button, PA6 and PA7 to rotary encoder.
			_encoder(GPIOA, TIM3, GPIO_Pin_5, GPIO_Pin_6 | GPIO_Pin_7),
			// Columns start with pin PC0, rows start with PC4
			_keypad(GPIOC, GPIO_PinSource0, GPIO_PinSource4),
			// LCD display, RS pin should be connected to PB5, R/W to PB6 and E to PB7
			// D0-D7 should be connected to PB8-PB15
			_lcd(GPIOB, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIOB,
			GPIO_PinSource8),
			// TODO:
			_stepper(GPIOA, GPIO_Pin_8, TIM4, TIM15)
	{
	}

	void setup()
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

		// Setup SysTick handler and util module
		systick::setup();
		util::setup();

		// Setup all used hardware
		_switch5.initialize();
		_encoder.initialize();
		_keypad.initialize();
		_lcd.initialize();
		_stepper.initialize();
	}

	void run()
	{

		_lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
		_encoder.limit(20);
		_lcd.clear();
		while (1)
		{
			_lcd << lcd::position(0, 0) << "Switch: " << _switch5.position() << ' '
					<< radix<2>((GPIOC->IDR >> GPIO_PinSource10) & 7);
			_lcd << lcd::position(0, 1) << "Encoder: "
					<< (_encoder.pressed() ? 'P' : 'N') << ' '
					<< _encoder.position() << "  ";
			_lcd << lcd::position(0, 2) << "Keypad: " << (char) _keypad.key();
			util::delay_ms(100);
		}
	}

	static application& instance()
	{
		return g_app;
	}
private:
	switch5 _switch5;
	encoder _encoder;
	keypad _keypad;
	lcd::HD44780 _lcd;
	stepper _stepper;
private:
	static application g_app;
};

application application::g_app;

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main()
{
	application::instance().setup();
	application::instance().run();
	return 0;
}

// IRQ
extern "C" void __attribute__ ((section(".after_vectors")))
SysTick_Handler()
{
	systick::tick();
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM4_IRQHandler()
{
	application::instance()._stepper.interrupt();
}
