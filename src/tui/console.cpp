#include "tui/console.hpp"

constexpr char tui::console::inputs_state::ButtonsMap[];

void tui::console::initialize()
{
	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Setup timer for delays
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(_debounce_timer);

	// Increment timer every 1ms
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 5; // Overflow every 5ms
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(_debounce_timer, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(_debounce_timer, TIM_IT_Update);
	TIM_ITConfig(_debounce_timer, TIM_IT_Update, ENABLE);
	TIM_Cmd(_debounce_timer, ENABLE);
}

void tui::console::debounce()
{
	// Query all hardware inputs and apply debouncing to them

	// No debouncing for encoder position -- should be handled by the timer itself.
	_current._enc_position = _encoder.raw_position();

	// Keypad
	_keypad_debounce <<= 8;
	_keypad_debounce |= static_cast<uint8_t>(_keypad.raw_key());
	uint16_t low = _keypad_debounce & 0xffff;
	if ((_keypad_debounce >> 16) == low)
	{
		uint8_t lowlow = low & 0xff;
		if ((low >> 8) == lowlow)
		{
			_current._keypad = lowlow;
		}
	}

	// Encoder buttons & control buttons debouncing

	_buttons_debounce <<= 4;
	_buttons_debounce |= (_buttons.raw_buttons() & 7);
	if (_encoder.raw_pressed())
		_buttons_debounce |= inputs_state::EncoderBit;

	uint16_t bd = _buttons_debounce;
	// Set bits which are '1' in last four samples
	uint8_t set = (bd >> 12) & (bd >> 8) & (bd >> 4) & bd & 0xf;
	_current._buttons |= set;
	// Reset bits which are '0' in last four samples
	uint8_t reset = (bd >> 12) | (bd >> 8) | (bd >> 4) | bd;
	_current._buttons &= reset;
}

tui::console::Event tui::console::read()
{
	Event event;
	event.kind = Nothing;

	inputs_state curr = _current;
	// Encoder
	if (_last._enc_position != curr._enc_position)
	{
		event.kind = EncoderMove;
		event.position = curr._enc_position;

		_last._enc_position = curr._enc_position;
	}
	else if (_last._keypad != curr._keypad)
	{
		if (_last._keypad == hw::keypad::None)
		{
			event.kind = ButtonPressed;
			event.key = curr._keypad;
			_last._keypad = curr._keypad;
		}
		else
		{
			event.kind = ButtonUnpressed;
			event.key = _last._keypad;

			// If current is not 'None', make it as one to convert button change
			// into two events (unpressed and then pressed).
			_last._keypad = hw::keypad::None;
		}
	}
	else if (_last._buttons != curr._buttons)
	{
		// Scan all button bits, report one button at a time
		for (int i = 0; i < 4; i++)
		{
			uint8_t mask = (1 << i);
			bool l = _last._buttons & mask;
			bool c = curr._buttons & mask;
			if (c && !l)
			{
				event.kind = ButtonPressed;
				event.key = inputs_state::ButtonsMap[i];
				_last._buttons |= mask;
				break;
			}
			else if (!c && l)
			{
				event.kind = ButtonUnpressed;
				event.key = inputs_state::ButtonsMap[i];
				_last._buttons &= ~mask;
				break;
			}
		}
	}
	return event;
}
