#include "input/input.hpp"

void input::debounce()
{
	// Query all hardware inputs and apply debouncing to them

	// No debouncing for encoder position -- should be handled by the timer itself.
	_current._enc_position = _encoder.raw_position();

	// Encoder button
	_enc_debounce <<= 1;
	_enc_debounce |= _encoder.raw_pressed() ? 1 : 0;
	_enc_debounce &= 0xf; // Use four last bits
	if (_enc_debounce == 0xf)
	{
		_current._enc_pressed = true;
	}
	else if (_enc_debounce == 0)
	{
		_current._enc_pressed = false;
	}

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

	// 5 position switch debouncing
	_switch_debounce <<= 4;
	_switch_debounce |= (_switch5.raw_position() & 0xf);
	switch (_switch_debounce) {
	case 0: _current._switch5 = 0; break;
	case 0x1111: _current._switch5 = 1; break;
	case 0x2222: _current._switch5 = 2; break;
	case 0x3333: _current._switch5 = 3; break;
	case 0x4444: _current._switch5 = 4; break;
	case 0x5555: _current._switch5 = 5; break;
	}
}

input::Event input::read()
{
	Event event;
	event.kind = Nothing;

	State curr = _current;
	// Encoder
	if (_last._enc_position != curr._enc_position)
	{
		event.kind = EncoderMove;
		event.position = curr._enc_position;
	}
	else if (_last._enc_pressed != curr._enc_pressed)
	{
		event.kind = EncoderButton;
		event.pressed = curr._enc_pressed;
	}
	else if (_last._keypad != curr._keypad)
	{
		event.kind = Keypad;
		event.key = curr._keypad;
	}
	else if (_last._switch5 != curr._switch5)
	{
		event.kind = Switch5;
		event.switch5 = curr._switch5;
	}
	_last = curr;
	return event;
}
