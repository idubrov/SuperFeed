#ifndef __KEYPAD_HPP
#define __KEYPAD_HPP

#include "stm32f10x.h"

namespace hw
{

class keypad
{
public:
	enum Key
	{
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
	constexpr keypad(GPIO_TypeDef* port, unsigned columns, unsigned rows) :
			_port(port), _columns(columns), _rows(rows)
	{
	}
	keypad(keypad const&) = delete;

	void initialize();

	char raw_key() const;
private:
	inline unsigned column_state() const
	{
		return (_port->IDR >> _columns) & 0x0f;
	}
	unsigned from_state(unsigned y, unsigned xstate) const;

private:
	GPIO_TypeDef* const _port;
	unsigned const _columns;
	unsigned const _rows;
};
}

template<typename S>
inline S const& operator<<(S const& l, hw::keypad const& keypad)
{
	l << "K: " << keypad.raw_key();
	return l;
}

#endif /* __KEYPAD_HPP */
