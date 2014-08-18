#include "tui/console.hpp"

constexpr char tui::console::ButtonsMap[];

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

	// Upload custom character
	upload_characters();

	// Reset encoder
	_encoder.set_limit(30000); // Arbitrary large number
	_encoder.set_position(0);
}

void tui::console::upload_characters()
{
	uint8_t up[8] =
	{ 0x00, 0x4, 0x0e, 0x1f, 0x04, 0x04, 0x04, 0x00 };
	_lcd.upload_character(UpArrowCharacter, up);
	uint8_t down[8] =
	{ 0x00, 0x4, 0x04, 0x04, 0x1f, 0x0e, 0x04, 0x00 };
	_lcd.upload_character(DownArrowCharacter, down);
	uint8_t updown[8] =
	{ 0x00, 0x4, 0x0e, 0x04, 0x04, 0x0e, 0x04, 0x00 };
	_lcd.upload_character(UpDownArrowCharacter, updown);
	uint8_t back[8] =
	{ 0x00, 0x00, 0x01, 0x09, 0x1f, 0x08, 0x00, 0x00 };
	_lcd.upload_character(BackCharacter, back);
}

void tui::console::debounce()
{
	// Query all hardware inputs and apply debouncing to them

	uint_fast32_t current = _current.load(std::memory_order_relaxed);

	// No debouncing for encoder position -- should be handled by the timer itself.
	current &= ~EncoderMask;
	current |= (_encoder.raw_position() << EncoderShift) & EncoderMask;

	// Keypad
	_keypad_debounce <<= 8;
	_keypad_debounce |= static_cast<uint8_t>(_keypad.raw_key());
	uint16_t low = _keypad_debounce & 0xffff;
	if ((_keypad_debounce >> 16) == low)
	{
		uint8_t lowlow = low & 0xff;
		if ((low >> 8) == lowlow)
		{
			current &= ~KeypadMask;
			current |= (lowlow << KeypadShift) & KeypadMask;
		}
	}

	// Encoder buttons & control buttons debouncing

	_buttons_debounce <<= 4;
	_buttons_debounce |= (_buttons.raw_buttons() & 7);
	if (_encoder.raw_pressed())
		_buttons_debounce |= EncoderBit;

	uint16_t bd = _buttons_debounce;
	// Set bits which are '1' in last four samples
	uint_fast32_t set = (bd >> 12) & (bd >> 8) & (bd >> 4) & bd & 0xf;
	current |= set << ButtonsShift;
	// Reset bits which are '0' in last four samples
	uint_fast32_t reset = (bd >> 12) | (bd >> 8) | (bd >> 4) | bd;
	current &= ~ButtonsMask | (reset << ButtonsShift);

	_current.store(current, std::memory_order_relaxed);
}

tui::console::Event tui::console::read()
{
	Event event;
	event.kind = Nothing;

	uint_fast32_t curr = _current;
	uint_fast16_t last_enc = (_last & EncoderMask) >> EncoderShift;
	uint_fast16_t curr_enc = (curr & EncoderMask) >> EncoderShift;
	uint_fast8_t last_buttons = (_last & ButtonsMask) >> ButtonsShift;
	uint_fast8_t curr_buttons = (curr & ButtonsMask) >> ButtonsShift;
	uint_fast8_t last_keypad = (_last & KeypadMask) >> KeypadShift;
	uint_fast8_t curr_keypad = (curr & KeypadMask) >> KeypadShift;

	// Encoder
	if (last_enc != curr_enc)
	{
		event.kind = EncoderMove;
		int16_t delta = curr_enc - last_enc;
		if (delta < -15000)
			delta += 30000;
		else if (delta > 15000)
			delta -= 30000;
		event.delta = delta;

		_last &= ~EncoderMask;
		_last |= curr & EncoderMask;
	}
	else if (last_keypad != curr_keypad)
	{
		if (last_keypad == hw::keypad::None)
		{
			event.kind = ButtonPressed;
			event.key = curr_keypad;
			_last &= ~KeypadMask;
			_last |= curr & KeypadMask;
		}
		else
		{
			event.kind = ButtonUnpressed;
			event.key = last_keypad;

			// If current is not 'None', make it as one to convert button change
			// into two events (unpressed and then pressed).
			_last &= ~KeypadMask;
			_last |= (static_cast<uint8_t>(hw::keypad::None) << KeypadShift)
					& KeypadMask;
		}
	}
	else if ((_last & ButtonsMask) != (curr & ButtonsMask))
	{
		// Scan all button bits, report one button at a time
		for (int i = 0; i < 4; i++)
		{
			uint_fast32_t mask = (1 << i);
			bool l = (last_buttons & mask) != 0;
			bool c = (curr_buttons & mask) != 0;
			if (c && !l)
			{
				event.kind = ButtonPressed;
				event.key = ButtonsMap[i];
				_last |= mask << ButtonsShift;
				break;
			}
			else if (!c && l)
			{
				event.kind = ButtonUnpressed;
				event.key = ButtonsMap[i];
				_last &= ~ButtonsMask | ((~mask) << ButtonsShift);
				break;
			}
		}
	}
	return event;
}
