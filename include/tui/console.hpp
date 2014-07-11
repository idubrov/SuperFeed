#ifndef __CONSOLE_HPP
#define __CONSOLE_HPP

#include "stm32f10x.h"

#include <atomic>

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
	// Custom characters
	constexpr static char UpArrowCharacter = 0x01;
	constexpr static char DownArrowCharacter = 0x02;

	// Buttons to key mappings
	enum Button
	{
		// Note that these should not intersect with keypad buttons
		LeftButton = '<',
		RightButton = '>',
		SelectButton = 'S',
		EncoderButton = '.'
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
	// Console state (like encoder position and limit)
	struct console_state
	{
		console_state(console& console) :
				_console(console), _moved(false), _encoder_state(
						console.get_encoder_state())
		{
		}
		console_state(console_state const&) = delete;
		console_state& operator=(console_state const&) = delete;
		console_state& operator=(console_state&&) = delete;

		console_state(console_state&& other) :
				_console(other._console), _moved(false), _encoder_state(
						other._encoder_state)
		{
			other._moved = true;
		}

		~console_state()
		{
			if (!_moved)
			{
				_console.set_encoder_state(_encoder_state);
			}
		}
	private:
		console& _console;bool _moved;
		uint32_t _encoder_state;
	};
private:
	// Bit to button key mapping
	constexpr static char ButtonsMap[] = { LeftButton, SelectButton, RightButton, EncoderButton};
	enum ButtonBits {
		LeftBit = 1,
		SelectBit = 2,
		RightBit = 4,
		EncoderBit = 8
	};
public:
	console(hw::lcd::HD44780& lcd, TIM_TypeDef* debounce_timer,
			hw::encoder& encoder, hw::keypad& keypad, hw::buttons& buttons) :
			_lcd(lcd), _debounce_timer(debounce_timer), _encoder(encoder), _keypad(
					keypad), _buttons(buttons), _current(0), _last(0), _keypad_debounce(
					0), _buttons_debounce(0), _enc_debounce(0)
	{
		reset();
	}

	console(console&&) = delete;
	console(console const&) = delete;
	console& operator=(console&&) = delete;
	console& operator=(console const&) = delete;

	void initialize();

	void reset()
	{
		_last = (static_cast<uint8_t>(hw::keypad::None) << KeypadShift) & KeypadMask;
	}

	Event read();

	void debounce();

	// All supported inputs
	inline uint_fast16_t enc_position()
	{
		return (current() & EncoderMask) >> EncoderShift;
	}
	inline char keypad()
	{
		return (current() & KeypadMask) >> KeypadShift;
	}
	inline bool enc_pressed()
	{
		return buttons() & EncoderBit;
	}
	inline bool left_pressed()
	{
		return buttons() & LeftBit;
	}
	inline bool select_pressed()
	{
		return buttons() & SelectBit;
	}
	inline bool right_pressed()
	{
		return buttons() & RightBit;
	}
	inline void set_encoder_limit(uint16_t limit)
	{
		_encoder.set_limit(limit);
	}
	inline void set_encoder(uint_fast16_t limit, uint_fast16_t position)
	{
		_encoder.set_limit(limit);
		_encoder.set_position(position);
	}
	inline hw::lcd::HD44780 const& lcd() const
	{
		return _lcd;
	}
	inline console_state guard_state()
	{
		return console_state(*this);
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
	inline uint_fast32_t current() const
	{
		return _current.load(std::memory_order_relaxed);
	}
	inline uint_fast8_t buttons() const
	{
		return (current() & ButtonsMask) >> ButtonsShift;
	}
	void upload_characters();

private:
	// Outputs
	hw::lcd::HD44780& _lcd;

	// Inputs
	TIM_TypeDef* _debounce_timer;

	hw::encoder& _encoder;
	hw::keypad& _keypad;
	hw::buttons& _buttons;

	constexpr static unsigned EncoderMask = 0x0000ffff;
	constexpr static unsigned EncoderShift = 0;
	constexpr static unsigned ButtonsMask = 0x00ff0000;
	constexpr static unsigned ButtonsShift = 16;
	constexpr static unsigned KeypadMask = 0xff000000;
	constexpr static unsigned KeypadShift = 24;
	// Current inputs state
	// The layout is the following (lowest to highest):
	// 16 bits: encoder
	// 8 bits: buttons
	// 8 bits: keypad
	std::atomic_uint_fast32_t _current; // Updated in IRQ
	uint_fast32_t _last; // Only used in main loop

	// Debouncing state (only used in IRQ handler)
	uint32_t _keypad_debounce;
	uint16_t _buttons_debounce;
	uint8_t _enc_debounce;
};
}

#endif /* __CONSOLE_HPP */
