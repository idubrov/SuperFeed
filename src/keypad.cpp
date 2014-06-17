#include "keypad.hpp"
#include "systick.hpp"
#include "delegate.hpp"

using namespace ::delegate;

constexpr keypad::Key keypad::c_mappings[];

void keypad::initialize()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Columns, input
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0x0f << _columns;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Rows, output
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0x0f << _rows;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Set all rows to zero
	GPIO_ResetBits(_port, 0x0f << _rows);

	systick::bind(Delegate<void()>::from<keypad, &keypad::scan>(this));
}

void keypad::scan() {
	// Only change current pressed key if last four keys match.
	_state <<= 8;
	_state |= raw_key();
	uint16_t low = _state & 0xffff;
	if ((_state >> 16) == low) {
		uint8_t lowlow = low & 0xff;
		if ((low >> 8) == lowlow) {
			_pressed = c_mappings[lowlow];
		}
	}
}

uint8_t keypad::from_state(uint8_t offset, uint8_t xstate) const {
	switch (xstate) {
	case 14: // 1110
		return offset + 4;
	case 13: // 1101
		return offset + 3;
	case 11: // 1011
		return offset + 2;
	case 7: // 0111
		return offset + 1;
	}
	// Multiple columns match, return nothing
	return 0;

}

uint8_t keypad::raw_key() const {
	uint8_t xstate = column_state();
	if (xstate == 0xf) {
		// All '1', no buttons are pressed
		return 0;
	}

	// Let's find row by scanning
	for (int y = 0; y < 4; y++) {
		_port->BSRR = (1 << (y + _rows)); // Set row to '1', to check if column state would change back to all '1's
		uint8_t xstate2 = column_state();
		_port->BRR = (1 << (y + _rows)); // Set row back to '0'

		// Either key was depressed or we found matching row
		if (xstate2 == 0xf) {
			// Column state is the same (which means same key is still pressed)
			if (column_state() == xstate) {
				// Now we are sure we found matching row
				int offset = y << 2;
				return from_state(offset, xstate);
			}

			// Key was depressed or another key was pressed while we were scanning
			return 0;
		}
	}
	return 0;
}
