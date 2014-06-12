#ifndef __KEYPAD_HPP
#define __KEYPAD_HPP

#include "config.hpp"

class keypad {
public:
	enum Key {
		None = ' ',	// no button pressed
		Star = '*',
		N0 = '0',
		Hash = '#',
		D = 'D',
		N7 = '7',
		N8 = '8',
		N9 = '9',
		C = 'C',
		N4 = '4',
		N5 = '5',
		N6 = '6',
		B = 'B',
		N1 = '1',
		N2 = '2',
		N3 = '3',
		A = 'A'
	};

	static constexpr Key c_mappings[] =
		{ None, Star, N0, Hash, D, N7, N8, N9, C, N4, N5, N6, B, N1, N2, N3, A, };
public:
	keypad(GPIO_TypeDef* port, uint8_t columns, uint8_t rows);

	Key key();
private:
	Key from_state(uint8_t y, uint8_t xstate);
	uint8_t column_state() const {
		return (_port->IDR >> _columns) & 0x0f;
	}

private:
	GPIO_TypeDef* const _port;
	uint8_t const _columns;
	uint8_t const _rows;
};

#endif /* __KEYPAD_HPP */
