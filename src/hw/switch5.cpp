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
}

uint8_t hw::switch5::raw_position() const
{
	switch ((_port->IDR >> _first_pin) & 7)
	{
	case 3:
		return RR;
	case 1:
		return R;
	case 5:
		return M;
	case 4:
		return L;
	case 6:
		return LL;
	default:
		return None;
	}
}
