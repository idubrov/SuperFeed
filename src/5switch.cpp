#include "5switch.hpp"

using namespace ::cfg::sw5;

switch5::switch5()
{
	RCC_APB2PeriphClockCmd(PortClock, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = Pins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(Port, &GPIO_InitStructure);
}

switch5::Position switch5::position() {
	uint16_t state = (Port->IDR & Pins) >> PinShift;
	switch (state) {
	case 3: return LL;
	case 1: return L;
	case 5: return M;
	case 4: return R;
	case 6: return RR;
	default: return NONE;
	}
}
