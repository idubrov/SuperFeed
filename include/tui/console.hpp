#ifndef __CONSOLE_HPP
#define __CONSOLE_HPP

#include "stm32f10x.h"

// Low-level input/output hardware
#include "hw/buttons.hpp"
#include "hw/encoder.hpp"
#include "hw/keypad.hpp"
#include "hw/lcd.hpp"

namespace tui
{

class console
{
public:
	enum Button
	{
		// Note that these should not intersect with keypad buttons
		LeftButton = '<',
		RightButton = '>',
		SelectButton = '.',
		EncoderButton = '@'
	};
	enum Kind
	{
		Nothing, ButtonPressed, ButtonUnpressed, EncoderMove
	};
	struct Event
	{
		Kind kind;
		union
		{
			uint16_t position;
			char key;
		};
	};
private:
	struct state
	{
		state(console& console) :
				_console(console), _moved(false), _encoder_state(
						console.get_encoder_state())
		{
		}
		state(state const&) = delete;
		state& operator=(state const&) = delete;
		state& operator=(state&&) = delete;

		state(state&& other) :
				_console(other._console), _moved(false), _encoder_state(
						other._encoder_state)
		{
			other._moved = true;
		}

		~state()
		{
			if (!_moved)
			{
				_console.set_encoder_state(_encoder_state);
			}
		}
	private:
		console& _console;
		bool _moved;
		uint32_t _encoder_state;
	};
private:
	struct InputState
	{
		// Bit to button key
		constexpr static char ButtonsMap[] = { LeftButton, SelectButton, RightButton, EncoderButton};
		enum ButtonBits {
			LeftBit = 1,
			SelectBit = 2,
			RightBit = 4,
			EncoderBit = 8
		};

		constexpr InputState() :
				_enc_position(0), _keypad(' '), _buttons(0)
		{
		}

		InputState(InputState const volatile& state) :
				_enc_position(state._enc_position), _keypad(state._keypad), _buttons(
						state._buttons)
		{
		}

		InputState& operator=(InputState const volatile & state)
		{
			*this = InputState(state);
			return *this;
		}
		InputState& operator=(const InputState&) = default;

		uint16_t _enc_position;
		char _keypad;
		uint8_t _buttons; // Buttons & encoder button
	};
public:
	console(hw::lcd::HD44780& lcd, TIM_TypeDef* debounce_timer,
			hw::encoder& encoder, hw::keypad& keypad, hw::buttons& buttons) :
			_lcd(lcd), _debounce_timer(debounce_timer), _encoder(encoder), _keypad(
					keypad), _buttons(buttons), _current(), _last(), _keypad_debounce(
					0), _buttons_debounce(0), _enc_debounce(0)
	{
	}

	void initialize();

	void reset()
	{
		_last = _current;
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
		return _current._buttons & InputState::EncoderBit;
	}
	inline char keypad()
	{
		return _current._keypad;
	}
	inline bool left_pressed()
	{
		return _current._buttons & InputState::LeftBit;
	}
	inline bool select_pressed()
	{
		return _current._buttons & InputState::SelectBit;
	}
	inline bool right_pressed()
	{
		return _current._buttons & InputState::RightBit;
	}
	inline void set_encoder_limit(uint16_t limit)
	{
		_encoder.set_limit(limit);
	}
	inline hw::lcd::HD44780 const& lcd() const
	{
		return _lcd;
	}
	inline state guard_state()
	{
		return state(*this);
	}
private:
	inline void set_encoder_state(uint32_t state)
	{
		_encoder.set_state(state);
	}
	inline uint32_t get_encoder_state()
	{
		return _encoder.get_state();
	}

private:
	// Outputs
	hw::lcd::HD44780& _lcd;

	// Inputs
	TIM_TypeDef* _debounce_timer;

	hw::encoder& _encoder;
	hw::keypad& _keypad;
	hw::buttons& _buttons;

	// State
	InputState volatile _current;
	InputState _last;

	// Debouncing state (only used in IRQ handler)
	uint32_t _keypad_debounce;
	uint16_t _buttons_debounce;
	uint8_t _enc_debounce;
};
}

#endif /* __CONSOLE_HPP */
