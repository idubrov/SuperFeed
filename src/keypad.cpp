#include "keypad.hpp"

constexpr keypad::Key keypad::c_mappings[];

keypad::keypad(GPIO_TypeDef* port, uint8_t columns, uint8_t rows) :
		_port(port), _columns(columns), _rows(rows) {
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
}

keypad::Key keypad::from_state(uint8_t offset, uint8_t xstate) {
	uint8_t idx = offset;
	switch (xstate) {
	case 14: // 1110
		idx += 4;
		break;
	case 13: // 1101
		idx += 3;
		break;
	case 11: // 1011
		idx += 2;
		break;
	case 7: // 0111
		idx += 1;
		break;
	default:
		idx = 0;
	}
	// Multiple columns match, return nothing
	return c_mappings[idx];

}

keypad::Key keypad::key() {
	uint8_t xstate = column_state();
	if (xstate == 0xf) {
		// All '1', no buttons are pressed
		return None;
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
			return None;
		}
	}
	return None;
}
