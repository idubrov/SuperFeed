#include "hw/keypad.hpp"

constexpr hw::keypad::Key hw::keypad::c_mappings[];

void hw::keypad::initialize()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Columns, input
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0x0f << _columns;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Rows, output
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0x0f << _rows;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Set all rows to one
	GPIO_SetBits(_port, 0x0f << _rows);
}

unsigned hw::keypad::from_state(unsigned offset, unsigned xstate) const {
	switch (xstate) {
	case 1: // 0001
		return offset + 4;
	case 2: // 0010
		return offset + 3;
	case 4: // 0100
		return offset + 2;
	case 8: // 1000
		return offset + 1;
	}
	// Multiple columns match, return nothing
	return 0;

}

char hw::keypad::raw_key() const {
	uint_fast8_t xstate = column_state();
	if (xstate == 0) {
		// All '0', no buttons are pressed
		return None;
	}

	// Let's find row by scanning
	for (unsigned y = 0; y < 4; y++) {
		// Set row to '0', to check if column state would change back to all '0's
		_port->BRR = (1 << (y + _rows));
		uint8_t xstate2 = column_state();
		// Set row back to '1'
		_port->BSRR = (1 << (y + _rows));

		// Either key was depressed or we found matching row
		if (xstate2 == 0) {
			// Column state is the same (which means same key is still pressed)
			if (column_state() == xstate) {
				// Now we are sure we found matching row
				int offset = y << 2;
				uint8_t key = from_state(offset, xstate);
				return c_mappings[key];
			}

			// Key was depressed or another key was pressed while we were scanning
			return None;
		}
	}
	return None;
}
