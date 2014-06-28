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
		Nothing,
		Keypad,
		EncoderMove,
		EncoderButton,
		Switch5
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
	input(encoder const& encoder, keypad const& keypad, switch5 const& switch5) :
			_encoder(encoder), _keypad(keypad), _switch5(switch5),
			_last_position(0), _last_pressed(false), _last_key(keypad::None),
			_last_switch(switch5::None)
	{
	}

	Event read();

private:
	encoder const& _encoder;
	keypad const& _keypad;
	switch5 const& _switch5;

	// Last state
	uint16_t _last_position;
	bool _last_pressed;
	char _last_key;
	switch5::Position _last_switch;
};

#endif /* __INPUT_HPP */
