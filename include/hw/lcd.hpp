#ifndef __LCD_HPP
#define __LCD_HPP

#include "util.hpp"
#include "simstream.hpp"

namespace hw
{
namespace lcd
{
// Constants
enum FunctionMode
{
	Bit4 = 0x00, Bit8 = 0x10
};
enum FunctionDots
{
	Dots5x8 = 0x00, Dots5x10 = 0x04
};
enum FunctionLine
{
	Line1 = 0x00, Line2 = 0x08
};
enum DisplayBlink
{
	BlinkOff = 0x00, BlinkOn = 0x01
};
enum DisplayCursor
{
	CursorOff = 0x00, CursorOn = 0x02
};
enum DisplayMode
{
	DisplayOff = 0x00, DisplayOn = 0x04
};
enum Direction
{
	Left = 0x00, Right = 0x04
};
enum Scroll
{
	CursorMove = 0x00, DisplayMove = 0x08
};
enum EntryModeDirection
{
	EntryLeft = 0x00, EntryRight = 0x02
};
enum EntryModeShift
{
	NoShift = 0x00, Shift = 0x01
};

enum Command
{
	ClearDisplay = 0x01,
	ReturnHome = 0x02,
	EntryModeSet = 0x04,
	DisplayControl = 0x08,
	CursorShift = 0x10,
	FunctionSet = 0x20,
	SetCGRamAddr = 0x40,
	SetDDRamAddr = 0x80
};

// HD44780 LCD screen controller
class HD44780
{
public:
public:
	constexpr HD44780(GPIO_TypeDef* control_port, uint16_t rs_pin,
			uint16_t rw_pin, uint16_t e_pin, GPIO_TypeDef* data_port,
			uint8_t data_shift, FunctionMode mode, bool use_busy = false) :
			_control_port(control_port), _data_port(data_port), _rs_pin(rs_pin), _rw_pin(
					rw_pin), _e_pin(e_pin), _data_shift(data_shift), _mode(
					mode), _use_busy(use_busy)
	{
	}
	HD44780(HD44780 const&) = delete;

	void initialize() const;

	inline void clear() const
	{
		command(ClearDisplay);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	void home() const
	{
		command(ReturnHome);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	void display(DisplayMode display, DisplayCursor cursor,
			DisplayBlink blink) const
	{
		command(DisplayControl | display | cursor | blink);
	}

	void entry_mode(EntryModeDirection dir, EntryModeShift scroll) const
	{
		command(EntryModeSet | dir | scroll);
	}

	void scroll(Direction dir) const
	{
		command(CursorShift | DisplayMove | dir);
	}

	void cursor(Direction dir) const
	{
		command(CursorShift | CursorMove | dir);
	}

	void position(uint8_t col, uint8_t row) const
	{
		uint8_t offset = 0;
		switch (row)
		{
		case 1:
			offset = 0x40;
			break;
		case 2:
			offset = 0x14;
			break;
		case 3:
			offset = 0x54;
			break;
		}
		command(SetDDRamAddr | (col + offset));
	}

	void write(char data) const;
private:
	void command(uint8_t cmd) const;
	void wait_busy_flag() const;

	// Typical command wait time is 37us
	void wait_ready(uint16_t delay = 50) const
	{
		if (_use_busy)
		{
			wait_busy_flag();
		}
		else
		{
			util::delay_us(delay);
		}
	}

	inline void pulse_enable() const
	{
		_control_port->BSRR = _e_pin;
		util::delay_us(1); // minimum delay is 450 ns
		_control_port->BRR = _e_pin;
	}

	static inline void wait_address()
	{
		// Address set up time is 40ns minimum (tAS)
		// Each our tick is 40ns (24 Mhz)
		// So, let's wait for 1 cycle
		__NOP();
	}

	inline void send(uint8_t data) const
	{
		if (_mode == Bit8)
			send8bits(data);
		else
		{
			send4bits(data >> 4);
			send4bits(data & 0xf);
		}
	}

	inline void send8bits(uint8_t data) const
	{
		_data_port->BSRR = ((uint16_t) data) << _data_shift; // Set '1's
		_data_port->BRR = ((uint16_t) (~data)) << _data_shift; // Clear '0's
		pulse_enable();
	}

	inline void send4bits(uint8_t data) const
	{
		_data_port->BSRR = ((uint16_t) (data & 0xf)) << _data_shift; // Set '1's
		_data_port->BRR = ((uint16_t) ((~data) & 0xf)) << _data_shift; // Clear '0's
		pulse_enable();
	}
private:
	GPIO_TypeDef* const _control_port;
	GPIO_TypeDef* const _data_port;
	uint16_t const _rs_pin;
	uint16_t const _rw_pin;
	uint16_t const _e_pin;
	uint8_t const _data_shift;
	FunctionMode const _mode;
	bool const _use_busy;
};

// Stream API for LCDscreen

struct __clear
{
};
inline __clear clear()
{
	return __clear();
}

inline HD44780 const& operator<<(HD44780 const& l, __clear)
{
	l.clear();
	return l;
}

struct __position
{
	uint8_t col;
	uint8_t row;
};
inline __position position(uint8_t col, uint8_t row)
{
	return
	{	col, row};
}

inline HD44780 const& operator<<(HD44780 const& l, __position p)
{
	l.position(p.col, p.row);
	return l;
}

}
}

#endif /* __LCD_HPP */
