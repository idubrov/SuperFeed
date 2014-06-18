#include "main.hpp"
#include "encoder.hpp"
#include "5switch.hpp"
#include "keypad.hpp"
#include "systick.hpp"
#include "util.hpp"
#include "HD44780.hpp"
#include "stepper.hpp"
#include "simstream.hpp"
#include "stm32f10x_gpio.h"

extern "C"
{
void SysTick_Handler();
void TIM1_UP_TIM16_IRQHandler(); // STEP pulse generation
}

lcd::HD44780* g_lcd;

// First, enable clocks for utilized subsystems
class application
{
	friend void ::TIM1_UP_TIM16_IRQHandler();
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
			// Stepper driver, PA8 should be connected to STEP, PA10 to DIR,
			// PA11 to ENABLE and PA12 to RESET.
			// ENABLE and RESET are active high (i.e, driver is enabled when both are high).
			_stepper(stepper::hw(GPIOA, GPIO_Pin_8, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, TIM1),
					stepper::delays(::cfg::stepper::StepLen,
							::cfg::stepper::StepSpace,
							::cfg::stepper::DirectionSetup,
							::cfg::stepper::DirectionHold))
	{
	}

	void setup()
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

		// Setup interrupt for pulse timer
		NVIC_InitTypeDef timerIT;
		timerIT.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
		timerIT.NVIC_IRQChannelPreemptionPriority = 0;
		timerIT.NVIC_IRQChannelSubPriority = 1;
		timerIT.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&timerIT);

		// Setup SysTick handler and util module
		systick::setup();
		util::setup();

		g_lcd = &_lcd;
		// Setup all used hardware
		_switch5.initialize();
		_encoder.initialize();
		_keypad.initialize();
		_lcd.initialize();
		_lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
		_stepper.initialize();
	}

	void run()
	{

		_encoder.limit(20);
		_lcd.clear();
		while (1)
		{
			_lcd << lcd::position(0, 0) << "Switch: " << _switch5.position()
					<< ' ' << radix<2>((GPIOC->IDR >> GPIO_PinSource10) & 7);
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
	stepper::controller _stepper;
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
TIM1_UP_TIM16_IRQHandler()
{
	application::instance()._stepper.step_completed();
}
