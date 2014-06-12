#ifndef __LCD_HPP
#define __LCD_HPP

#include "util.hpp"
#include "config.hpp"

class lcd {
public:
	enum FunctionMode {
		Bit8 = 0x10
	};
	enum FunctionDots {
		Dots5x8 = 0x00,
		Dots5x10 = 0x04
	};
	enum FunctionLine {
		Line1 = 0x00,
		Line2 = 0x08
	};
	enum DisplayBlink {
		BlinkOff = 0x00,
		BlinkOn = 0x01
	};
	enum DisplayCursor {
		CursorOff = 0x00,
		CursorOn = 0x02
	};
	enum DisplayMode {
		DisplayOff = 0x00,
		DisplayOn = 0x04
	};
	enum Direction {
		Left = 0x00,
		Right = 0x04
	};
	enum Scroll {
		CursorMove = 0x00,
		DisplayMove = 0x08
	};
	enum EntryModeDirection {
		EntryLeft = 0x00,
		EntryRight = 0x02
	};
	enum EntryModeShift{
		NoShift = 0x00,
		Shift = 0x01
	};

	enum Command {
		ClearDisplay = 0x01,
		ReturnHome = 0x02,
		EntryModeSet = 0x04,
		DisplayControl = 0x08,
		CursorShift = 0x10,
		FunctionSet = 0x20,
		SetCGRamAddr = 0x40,
		SetDDRamAddr = 0x80
	};
public:
	constexpr lcd(GPIO_TypeDef* control_port, uint16_t rs_pin, uint16_t rw_pin, uint16_t e_pin,
			GPIO_TypeDef* data_port, uint8_t data_shift, bool use_busy = false) :
			_control_port(control_port), _rs_pin(rs_pin), _rw_pin(rw_pin), _e_pin(e_pin),
			_data_port(data_port), _data_shift(data_shift), _use_busy(use_busy)
	{
	}
	lcd(lcd const&) = delete;

	void initialize();

	inline void clear() {
		command(ClearDisplay);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	void home() {
		command(ReturnHome);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	void display(DisplayMode display, DisplayCursor cursor, DisplayBlink blink) {
		command(DisplayControl | display | cursor | blink);
	}

	void entry_mode(EntryModeDirection dir, EntryModeShift scroll) {
		command(EntryModeSet | dir | scroll);
	}

	void scroll(Direction dir) {
		command(CursorShift | DisplayMove | dir);
	}

	void cursor(Direction dir) {
		command(CursorShift | CursorMove | dir);
	}

	void position(uint8_t col, uint8_t row) {
		uint8_t offset = 0;
		switch (row) {
		case 1: offset = 0x40; break;
		case 2: offset = 0x14; break;
		case 3: offset = 0x54; break;
		}
		command(SetDDRamAddr | (col + offset));
	}

	void print(char data);
	void print(char const* data);
private:
	void command(uint8_t cmd);
	void wait_busy_flag();

	// Typical command wait time is 37us
	void wait_ready(uint16_t delay = 50) {
		if (_use_busy) {
			wait_busy_flag();
		} else {
			util::delay_us(delay);
		}
	}

	inline void pulse_enable() {
		using namespace ::cfg::lcd;
		_control_port->BSRR = _e_pin;
		util::delay_us(1); // minimum delay is 450 ns
		_control_port->BRR = _e_pin;
	}

	static inline void wait_address() {
		// Address set up time is 40ns minimum (tAS)
		// Each our tick is 40ns (24 Mhz)
		// So, let's wait for 1 cycle
		__NOP();
	}

	inline void send(uint8_t data) {
		_data_port->BSRR = ((uint16_t)   data ) << _data_shift; // Set '1's
		_data_port->BRR =  ((uint16_t) (~data)) << _data_shift; // Clear '0's
		pulse_enable();
	}
private:
	GPIO_TypeDef* const _control_port;
	uint16_t const _rs_pin;
	uint16_t const _rw_pin;
	uint16_t const _e_pin;
	GPIO_TypeDef* const _data_port;
	uint8_t  const _data_shift;
	bool const _use_busy;
};

#endif /* __LCD_HPP */
