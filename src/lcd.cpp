#include "lcd.hpp"

void lcd::initialize() const
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Set control pins to output
	GPIO_InitStructure.GPIO_Pin = _rs_pin | _rw_pin | _e_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_control_port, &GPIO_InitStructure);

	// Set 8 data pins to output
	GPIO_InitStructure.GPIO_Pin = 0xff << _data_shift;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_data_port, &GPIO_InitStructure);

	// Run initialization procedure for the display. We use 8-bit mode here
	// Need to wait at least 40ms after Vcc rises to 2.7V
	util::delay_ms(50);

	_control_port->BRR = _rs_pin;
	wait_address();

	// Set to 8-bit mode, 2 line, 5x10 font
	// Display off, clear, entry mode set
	send(FunctionSet | Bit8 | Line2 | Dots5x10); // Send command for the first time
	util::delay_us(4500); // Wait for more than 4.1ms
	pulse_enable(); // Repeat for the second time
	util::delay_us(150); // Wait for more than 100us
	pulse_enable(); // Repeat for the third time

	// Now display should be properly initialized, we can check BF now
	// Though if we are not checking BF, waiting time is longer
	display(DisplayOff, CursorOff, BlinkOff);
	clear();
	entry_mode(EntryRight, NoShift);
}

void lcd::write(char data) const {
	_control_port->BSRR = _rs_pin;
	wait_address(); // tAS
	send(data);
	wait_ready();
	// It takes 4us more (tADD) to update address counter
	util::delay_us(5);
}

void lcd::command(uint8_t cmd) const {
	_control_port->BRR = _rs_pin;
	wait_address(); // tAS
	send(cmd);
	wait_ready();
}

void lcd::wait_busy_flag() const {
	// LCD has OD output, set all to '0' just to be sure.
	_data_port->BRR = 0xff << _data_shift;

	// First, set 8 data ports to input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xff << _data_shift;
	// LCD will pull-up to 5v, but it driving capability is poor, so we set pull-up to 3v here
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_data_port, &GPIO_InitStructure);

	// Set R/W to '1', RS to '0'
	_control_port->BSRR = _rw_pin;
	_control_port->BRR = _rs_pin;
	wait_address(); // tAS
	bool busy;
	do {
		_control_port->BSRR = _e_pin;
		util::delay_us(1); // minimum delay is 360+25 ns (tDDR + tER)
		busy = (_data_port->IDR & (0x80 << _data_shift)) != 0;
		util::delay_us(1); // minimum delay is 450ns (PWen)
		_control_port->BRR = _e_pin;
	} while(busy);
	// tAH is 10ns, which is less than one cycle. So we don't have to wait.

	// Set R/W back to '0'
	_control_port->BRR = _rw_pin;

	// Reset data port to output
	GPIO_InitStructure.GPIO_Pin = 0xff << _data_shift;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_data_port, &GPIO_InitStructure);
}
