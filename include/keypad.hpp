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
public:
	keypad(GPIO_TypeDef* port, uint8_t columns, uint8_t rows);

	Key key();
private:
	Key from_state(uint8_t y, uint8_t xstate);
	uint8_t column_state() const
	{
		return (_port->IDR >> _columns) & 0x0f;
	}

private:
	GPIO_TypeDef* const _port;
	uint8_t _columns;
	uint8_t _rows;
};

#endif /* __KEYPAD_HPP */
