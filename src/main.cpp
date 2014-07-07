#include "main.hpp"
#include "tui/console.hpp"
#include "simstream.hpp"
#include "stepper.hpp"
#include "eeprom.hpp"
#include "tui/menu/settings.hpp"

#include "stm32f10x_gpio.h"

using namespace ::hw;
using namespace ::tui;

extern "C"
{
void SysTick_Handler();
void TIM1_UP_TIM16_IRQHandler(); // STEP pulse generation
void TIM7_IRQHandler(); // Input debouncing

extern unsigned int __eeprom_start;
extern unsigned int __eeprom_pages;
}

// First, enable clocks for utilized subsystems
class application
{
	friend void ::TIM1_UP_TIM16_IRQHandler();
	friend void ::TIM7_IRQHandler();
public:
	application() :
			// Columns start with pin PA0, rows start with PA4
			_keypad(GPIOA, GPIO_PinSource0, GPIO_PinSource4),
			// PC10-PC12 should be connected to the first three output of the switch.
			// 4th lead of the switch should be connected to GND.
			_switch5(GPIOC, GPIO_PinSource10),
			// PB5 should be connected to encoder button, PB6 and PB7 to rotary encoder.
			_encoder(GPIOB, TIM4, GPIO_Pin_5, GPIO_Pin_6 | GPIO_Pin_7),
			// LCD display, RS pin should be connected to PB15, R/W to PB8 and E to PB9
			// D4-D7 should be connected to PB8-PB15
			_lcd(GPIOB, GPIO_Pin_15, GPIO_Pin_8, GPIO_Pin_9, GPIOA,
			GPIO_PinSource8, lcd::Bit4),
			// Stepper driver, PB13 should be connected to STEP, PB12 to DIR,
			// PB11 to ENABLE and PB10 to RESET.
			// ENABLE and RESET are active high (i.e, driver is enabled when both are high).
			_driver(GPIOB, GPIO_Pin_13, GPIO_Pin_12, GPIO_Pin_11,
			GPIO_Pin_10, TIM1, ::cfg::stepper::StepLen), _stepper(_driver,
					stepper::delays(::cfg::stepper::StepLen,
							::cfg::stepper::StepSpace,
							::cfg::stepper::DirectionSetup,
							::cfg::stepper::DirectionHold)),
			// Use page 126 and 127 for persistent storage
			_eeprom((uint32_t) &__eeprom_start, (uint32_t) &__eeprom_pages), _console(
					_lcd, TIM7, _encoder, _keypad, _switch5), _settings(
					_console)
	{
	}

	void setup()
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // Stepper
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // Encoder
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // Utility timer
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // Debouncing timer

		// Setup interrupt for pulse timer (TIM1)
		NVIC_InitTypeDef timerIT;
		timerIT.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
		timerIT.NVIC_IRQChannelPreemptionPriority = 0;
		timerIT.NVIC_IRQChannelSubPriority = 1;
		timerIT.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&timerIT);

		// Setup interrupt for debounce timer (TIM7)
		timerIT.NVIC_IRQChannel = TIM7_IRQn;
		timerIT.NVIC_IRQChannelPreemptionPriority = 3; // lowest priority
		timerIT.NVIC_IRQChannelSubPriority = 3; // lowest priority
		timerIT.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&timerIT);

		// Setup core hardware (on-board LEDs and delay timer)
		core::setup();

		// Setup all used hardware
		_keypad.initialize();
		_switch5.initialize();
		_encoder.initialize();
		_console.initialize();
		_lcd.initialize();
		_lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
		_driver.initialize();
		_eeprom.initialize();

		_stepper.reset();
	}

	void run()
	{
		_lcd.clear();

		_settings.run();
//
//		while (1)
//			;
		while (1)
		{
			auto ev = _console.read();
			if (ev.kind == console::Nothing)
			{
				core::delay_ms(100);
				continue;
			}

			if (ev.kind == console::EncoderMove)
			{
				_lcd << "M";
			}
			else if (ev.kind == console::EncoderPressed)
			{
				_lcd << "B";
			}
			else if (ev.kind == console::EncoderUnpressed)
			{
				_lcd << "U";
			}
			else if (ev.kind == console::Keypad)
			{
				_lcd << "K";
			}
			else if (ev.kind == console::Switch5)
			{
				_lcd << "S";
			}
		}

//		// STEPPER.....
		bool pressed = false;
		uint8_t last = switch5::None;

		uint32_t thread = 8; // TPI of the thread we are cutting
		uint32_t RPM = 120 << 8; // Spindle speed in 24.8 format
		uint32_t fast = ::cfg::stepper::StepsPerInch * RPM / (60 * thread); // steps per sec
		uint32_t slow = fast / 3;

		while (1)
		{
			_lcd << lcd::position(0, 0) << _stepper;

			uint8_t pos = _switch5.raw_position();
			if (pos != last)
			{
				_stepper.set_speed(pos == switch5::LL ? fast : slow);
			}
			last = pos;

			if (_encoder.raw_pressed())
			{
				if (!pressed)
				{
					_stepper.move(1000);
				}
				pressed = true;
			}
			else
			{
				pressed = false;
			}
		}
		while (1)
			;
	}

	static application& instance()
	{
		return g_app;
	}
private:
	keypad _keypad;
	switch5 _switch5;
	encoder _encoder;
	lcd::HD44780 _lcd;
	driver _driver;
	stepper::controller _stepper;
	eeprom _eeprom;
	console _console;
	settings _settings;
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

// IRQ handlers
extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_UP_TIM16_IRQHandler()
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	application::instance()._stepper.step_completed();
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM7_IRQHandler()
{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	application::instance()._console.debounce();
}
