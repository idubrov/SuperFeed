#ifndef __INPUT_HPP
#define __INPUT_HPP

#include "stm32f10x.h"

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
			uint8_t switch5;
		};
	};
private:
	struct State
	{
		constexpr State() :
				_enc_position(0), _enc_pressed(false), _keypad(' '), _switch5(0)
		{
		}

		State(State const volatile& state) :
				_enc_position(state._enc_position), _enc_pressed(
						state._enc_pressed), _keypad(state._keypad), _switch5(
						state._switch5)
		{
		}

		State& operator=(State const volatile & state)
		{
			*this = State(state);
			return *this;
		}
		State& operator=(const State&) = default;

		uint16_t _enc_position;
		bool _enc_pressed;
		char _keypad;
		uint8_t _switch5;
	};
public:
	input(TIM_TypeDef* debounce_timer, hw::encoder& encoder, hw::keypad& keypad,
			hw::switch5& switch5) :
			_debounce_timer(debounce_timer), _encoder(encoder), _keypad(keypad), _switch5(
					switch5), _current(), _last(), _keypad_debounce(0), _switch_debounce(
					0), _enc_debounce(0)
	{
	}

	void initialize();

	void reset()
	{
		_last = _current;
	}

	hw::encoder& get_encoder()
	{
		return _encoder;
	}

	Event read();

	void debounce();

	// All supported inputs
	inline uint16_t enc_position()
	{
		return _current._enc_position;
	}
	inline bool enc_pressed()
	{
		return _current._enc_pressed;
	}
	inline char keypad()
	{
		return _current._keypad;
	}
	inline uint8_t switch5()
	{
		return _current._switch5;
	}

private:
	TIM_TypeDef* _debounce_timer;
	hw::encoder& _encoder;
	hw::keypad& _keypad;
	hw::switch5& _switch5;

	// State
	State volatile _current;
	State _last;

	// Debouncing state (only used in IRQ handler)
	uint32_t _keypad_debounce;
	uint16_t _switch_debounce;
	uint8_t _enc_debounce;
};

#endif /* __INPUT_HPP */
