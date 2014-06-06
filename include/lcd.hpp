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

	static inline void clear() {
		command(ClearDisplay);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	static void home() {
		command(ReturnHome);
		// This command could take as long as 1.52ms to execute
		wait_ready(2000);
	}

	static void display(DisplayMode display, DisplayCursor cursor, DisplayBlink blink) {
		command(DisplayControl | display | cursor | blink);
	}

	static void entry_mode(EntryModeDirection dir, EntryModeShift scroll) {
		command(EntryModeSet | dir | scroll);
	}

	static void scroll(Direction dir) {
		command(CursorShift | DisplayMove | dir);
	}

	static void cursor(Direction dir) {
		command(CursorShift | CursorMove | dir);
	}

	static void position(uint8_t col, uint8_t row) {
		uint8_t offset = 0;
		switch (row) {
		case 1: offset = 0x40; break;
		case 2: offset = 0x14; break;
		case 3: offset = 0x54; break;
		}
		command(SetDDRamAddr | (col + offset));
	}

	static void print(char data);
	static void print(char const* data);
private:
	static void command(uint8_t cmd);
	static void wait_busy_flag();

	// Typical command wait time is 37us
	static void wait_ready(uint16_t delay = 50) {
		if (::cfg::lcd::UseBusyFlag) {
			wait_busy_flag();
		} else {
			util::delay_us(delay);
		}
	}

	static inline void pulse_enable() {
		using namespace ::cfg::lcd;
		ControlPort->BSRR = EPin;
		util::delay_us(1); // minimum delay is 450 ns
		ControlPort->BRR = EPin;
	}

	static inline void wait_address() {
		// Address set up time is 40ns minimum (tAS)
		// Each our tick is 20ns (48 Mhz)
		// So, let's wait for 5 cycles
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}

	static inline void send(uint8_t data) {
		using namespace ::cfg::lcd;
		DataPort->BSRR = (((uint16_t)   data ) << DataShift) & DataPins; // Set '1's
		DataPort->BRR =  (((uint16_t) (~data)) << DataShift) & DataPins; // Clear '0's
		pulse_enable();
	}
private:
	static lcd g_instance;
	lcd();
};

#endif /* __LCD_HPP */
