#include "input/input.hpp"

input::Event input::read()
{
	Event event;
	event.kind = Nothing;

	// Encoder
	uint16_t enc = _encoder.raw_position();
	if (_last_position != enc)
	{
		_last_position = enc;
		event.kind = EncoderMove;
		event.position = enc;
		return event;
	}

	bool pressed = _encoder.raw_pressed();
	if (_last_pressed != pressed)
	{
		_last_pressed = pressed;
		event.kind = EncoderButton;
		event.pressed = pressed;
		return event;
	}

	// Keypad
	auto key = _keypad.raw_key();
	if (key != _last_key)
	{
		_last_key = key;
		event.kind = Keypad;
		event.key = key;
		return event;
	}

	// Switch5
	auto sw = _switch5.raw_position();
	if (_last_switch != sw)
	{
		_last_switch = sw;
		event.kind = Switch5;
		event.sw5 = sw;
		return event;
	}
	return event;
}
