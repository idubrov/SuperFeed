#include "keypad.hpp"

using namespace ::cfg::keypad;

void KPAD_Config()
{
	// Enable port C clock
	RCC_APB2PeriphClockCmd(PortClock, ENABLE);

	// Rows, output
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = RowsPins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port, &GPIO_InitStructure);

	// Set all rows to zero
	GPIO_ResetBits(Port, RowsPins);

	// Columns, input
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ColumnsPins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(Port, &GPIO_InitStructure);
}

keypad::Key keypad::from_state(uint8_t y, uint8_t xstate)
{
	switch (xstate)
	{
	case 14: // 1110
		return static_cast<Key>((y << 2) + 1);
	case 13: // 1101
		return static_cast<Key>((y << 2) + 2);
	case 11: // 1011
		return static_cast<Key>((y << 2) + 3);
	case 7: // 0111
		return static_cast<Key>((y << 2) + 4);
	}
	// Multiple columns match, return nothing
	return None;

}

keypad::Key keypad::key()
{
	uint8_t xstate = column_state();
	if (xstate == 0xf)
	{
		// All '1', no buttons are pressed
		return None;
	}

	// Let's find row by scanning
	for (int y = 0; y < 4; y++)
	{
		Port->BSRR = (1 << y); // Set row to '1', to check if column state would change back to all '1's
		uint8_t xstate2 = column_state();
		Port->BRR = (1 << y); // Set row back to '0'

		// Either key was depressed or we found matching row
		if (xstate2 == 0xf)
		{
			// Column state is the same (which means same key is still pressed)
			if (column_state() == xstate)
			{
				// Now we are sure we found matching row
				return from_state(y, xstate);
			}

			// Key was depressed or another key was pressed while we were scanning
			return None;
		}
	}
	return None;
}
