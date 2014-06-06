#include "lcd.hpp"

using namespace ::cfg::lcd;

lcd::lcd()
{
	// Enable port A and B clocks
	RCC_APB2PeriphClockCmd(ControlPortClock, ENABLE);
	RCC_APB2PeriphClockCmd(DataPortClock, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// Set control pins to output
	GPIO_InitStructure.GPIO_Pin = RSPin | RWPin | EPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ControlPort, &GPIO_InitStructure);

	// Set data pins to output
	GPIO_InitStructure.GPIO_Pin = DataPins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DataPort, &GPIO_InitStructure);

	// Run initialization procedure for the display. We use 8-bit mode here
	// Need to wait at least 40ms after Vcc rises to 2.7V
	util::delay_ms(50);

	ControlPort->BRR = RSPin;
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

void lcd::print(char data) {
	ControlPort->BSRR = RSPin;
	wait_address(); // tAS
	send(data);
	wait_ready();
	// It takes 4us more (tADD) to update address counter
	util::delay_us(5);
}

void lcd::print(char const* data) {
	while(*data) {
		print(*data++);
	}
}


void lcd::command(uint8_t cmd) {
	ControlPort->BRR = RSPin;
	wait_address(); // tAS
	send(cmd);
	wait_ready();
}

void lcd::wait_busy_flag() {
	// LCD has OD output, set all to '0' just to be sure.
	DataPort->BRR = DataPins;

	// First, set data ports to input
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DataPins;
	// LCD will pull-up to 5v, but it driving capability is poor, so we set pull-up to 3v here
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DataPort, &GPIO_InitStructure);

	// Set R/W to '1', RS to '0'
	ControlPort->BSRR = RWPin;
	ControlPort->BRR = RSPin;
	wait_address(); // tAS
	bool busy;
	do {
		ControlPort->BSRR = EPin;
		util::delay_us(1); // minimum delay is 360+25 ns (tDDR + tER)
		busy = (DataPort->IDR & BusyFlagPin);
		util::delay_us(1); // minimum delay is 450ns (PWen)
		ControlPort->BRR = EPin;
	} while(busy);
	// tAH is 10ns, which is less than one cycle. So we don't have to wait.

	// Set R/W back to '0'
	ControlPort->BRR = RWPin;

	// Reset data port to output
	GPIO_InitStructure.GPIO_Pin = DataPins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DataPort, &GPIO_InitStructure);
}
