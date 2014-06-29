#ifndef __INPUT_HPP
#define __INPUT_HPP

#include "config.hpp"
#include "5switch.hpp"
#include "encoder.hpp"
#include "keypad.hpp"

class input
{
public:
	enum Kind
	{
		Nothing, Keypad, EncoderMove, EncoderButton, Switch5
	};
	struct Event
	{
		Kind kind;
		union
		{
			uint16_t position;
			bool pressed;
			char key;
			switch5::Position sw5;
		};
	};

public:
	input(encoder& encoder, keypad& keypad, switch5& switch5) :
			_encoder(encoder), _keypad(keypad), _switch5(switch5), _last_position(
					0), _last_pressed(false), _last_key(keypad::None), _last_switch(
					switch5::None)
	{
	}

	void reset()
	{
		_last_position = _encoder.position();
		_last_pressed = _encoder.pressed();
		_last_key = _keypad.key();
		_last_switch = _switch5.position();
	}

	encoder& get_encoder()
	{
		return _encoder;
	}

	Event read();

private:
	encoder& _encoder;
	keypad& _keypad;
	switch5& _switch5;

	// Last state
	uint16_t _last_position;
	bool _last_pressed;
	char _last_key;
	switch5::Position _last_switch;
};

#endif /* __INPUT_HPP */
