#ifndef __KEYPAD_HPP
#define __KEYPAD_HPP

#include "config.hpp"

class keypad {
public:
	enum Key
	{
		None = 0	// no button pressed
		// FIXME-isd: 16 buttons constants, 1 to 16
	};

	static Key key();
private:
	static Key from_state(uint8_t y, uint8_t xstate);
	static uint8_t column_state()
	{
		using namespace ::cfg::keypad;
		return (Port->IDR & ColumnsPins) >> ColumnsShift;
	}

private:
	static keypad g_instance;
	keypad();
};

#endif /* __KEYPAD_HPP */
