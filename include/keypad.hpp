#ifndef __KEYPAD_HPP
#define __KEYPAD_HPP

#include "config.hpp"

class keypad {
public:
	enum Key
	{
		None = 0,	// no button pressed
		Star = 1,
		N0 = 2,
		Hash = 3,
		D = 4,
		N7 = 5,
		N8 = 6,
		N9 = 7,
		C = 8,
		N4 = 9,
		N5 = 10,
		N6 = 11,
		B = 12,
		N1 = 13,
		N2 = 14,
		N3 = 15,
		A = 16
	};

	static Key key();
private:
	static Key from_state(int y, int xstate);
	static int column_state()
	{
		using namespace ::cfg::keypad;
		return (Port->IDR & ColumnsPins) >> ColumnsShift;
	}

private:
	static keypad g_instance;
	keypad();
};

#endif /* __KEYPAD_HPP */
