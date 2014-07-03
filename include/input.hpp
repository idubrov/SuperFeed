#ifndef __INPUT_HPP
#define __INPUT_HPP

#include "config.hpp"
#include "hw/switch5.hpp"
#include "hw/encoder.hpp"
#include "hw/keypad.hpp"

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
			hw::switch5::Position sw5;
		};
	};

public:
	input(hw::encoder& encoder, hw::keypad& keypad, hw::switch5& switch5) :
			_encoder(encoder), _keypad(keypad), _switch5(switch5), _last_position(
					0), _last_pressed(false), _last_key(hw::keypad::None), _last_switch(
					hw::switch5::None)
	{
	}

	void reset()
	{
		_last_position = _encoder.position();
		_last_pressed = _encoder.pressed();
		_last_key = _keypad.key();
		_last_switch = _switch5.position();
	}

	hw::encoder& get_encoder()
	{
		return _encoder;
	}

	Event read();

private:
	hw::encoder& _encoder;
	hw::keypad& _keypad;
	hw::switch5& _switch5;

	// Last state
	uint16_t _last_position;
	bool _last_pressed;
	char _last_key;
	hw::switch5::Position _last_switch;
};

#endif /* __INPUT_HPP */
