#include "hw/lcd.hpp"

void hw::lcd::HD44780::initialize() const
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Set control pins to output
	GPIO_InitStructure.GPIO_Pin = _rs_pin | _rw_pin | _e_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_control_port, &GPIO_InitStructure);

	// Set 8 data pins to output
	GPIO_InitStructure.GPIO_Pin = (_mode == Bit8 ? 0xff : 0xf) << _data_shift;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_data_port, &GPIO_InitStructure);

	// Need to wait at least 40ms after Vcc rises to 2.7V
	core::delay_ms(50);

	_control_port->BRR = _rs_pin;
	wait_address();
	if (_mode == Bit8)
	{
		// Run initialization procedure for the display. We use 8-bit mode here

		// Set to 8-bit mode, 2 line, 5x10 font
		// Display off, clear, entry mode set
		send8bits(FunctionSet | Bit8 | Line2 | Dots5x10); // Send command for the first time
		core::delay_us(4500); // Wait for more than 4.1ms

		pulse_enable(); // Repeat for the second time
		core::delay_us(150); // Wait for more than 100us

		pulse_enable(); // Repeat for the third time
		wait_ready();
	}
	else
	{
		// Run initialization procedure for the display. We use 8-bit mode here
		send4bits((FunctionSet | Bit8) >> 4);
		core::delay_us(4500); // Wait for more than 4.1ms

		pulse_enable(); // Repeat for the second time
		core::delay_us(150); // Wait for more than 100us

		pulse_enable(); // Repeat for the third time
		wait_ready(); // Wait fo FunctionSet to finish

		// Now we switch to 4-bit mode
		send4bits((FunctionSet | Bit4) >> 4);
		wait_ready(); // Wait for FunctionSet to finish
	}

	// Finally, set # lines, font size
	command(FunctionSet | _mode | Line2 | Dots5x10);

	// Now display should be properly initialized, we can check BF now
	// Though if we are not checking BF, waiting time is longer
	display(DisplayOff, CursorOff, BlinkOff);
	clear();
	entry_mode(EntryRight, NoShift);
}

void hw::lcd::HD44780::write(char data) const
{
	_control_port->BSRR = _rs_pin;
	wait_address(); // tAS
	send(data);
	wait_ready();
	// It takes 4us more (tADD) to update address counter
	core::delay_us(5);
}

void hw::lcd::HD44780::command(uint_fast8_t cmd) const
{
	_control_port->BRR = _rs_pin;
	wait_address(); // tAS
	send(cmd);
	wait_ready();
}

void hw::lcd::HD44780::wait_busy_flag() const
{
	// LCD has OD output, set all to '0' just to be sure.
	_data_port->BRR = 0xff << _data_shift;

	// First, set 8 data ports to input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xff << _data_shift;
	// LCD will pull-up to 5v
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(_data_port, &GPIO_InitStructure);

	// Set R/W to '1', RS to '0'
	_control_port->BSRR = _rw_pin;
	_control_port->BRR = _rs_pin;
	wait_address(); // tAS
	bool busy;
	do
	{
		_control_port->BSRR = _e_pin;
		core::delay_us(1); // minimum delay is 360+25 ns (tDDR + tER)
		busy = (_data_port->IDR & (0x80 << _data_shift)) != 0;
		core::delay_us(1); // minimum delay is 450ns (PWen)
		_control_port->BRR = _e_pin;
	} while (busy);
	// tAH is 10ns, which is less than one cycle. So we don't have to wait.

	// Set R/W back to '0'
	_control_port->BRR = _rw_pin;

	// Reset data port to output
	GPIO_InitStructure.GPIO_Pin = 0xff << _data_shift;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_data_port, &GPIO_InitStructure);
}
