#include "hw/switch5.hpp"
#include "systick.hpp"

using namespace ::delegate;

void hw::switch5::initialize()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 7 << _first_pin; // We use 3 consecutive pins.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_port, &GPIO_InitStructure);

	systick::bind(Delegate<void()>::from<switch5, &switch5::scan>(this));
}

// Should be called from SysTick interrupt handler.
void hw::switch5::scan()
{
	// Software debouncing for 5 position switch. Shift state 3 bits and merge new
	// bits. Only if bits are the same for 10 last scans, change current position.
	_state <<= 3;
	_state |= (_port->IDR >> _first_pin) & 7;
	_state &= 0x3FFFFFFF;
	switch (_state) {
	case 0x1B6DB6DB: _position = RR; break; // 0b00011011011011011011011011011011
	case 0x9249249: _position = R; break;   // 0b00001001001001001001001001001001
	case 0x2DB6DB6D: _position = M; break;  // 0b00101101101101101101101101101101
	case 0x24924924: _position = L; break;  // 0b00100100100100100100100100100100
	case 0x36DB6DB6: _position = LL; break; // 0b00110110110110110110110110110110
	// Otherwise, transition or noise, ignore
	}
}
